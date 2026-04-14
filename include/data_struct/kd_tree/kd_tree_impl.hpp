template <typename T>
kd_tree<T>::kd_tree(const std::vector<value_type>& objs, NodeSplitter func, bool multithread)
: objs_(objs),
  root_(nullptr),
  size_(0),
  use_multithread_(multithread)
{
  createTree(objs, func);
}

template <typename T>
void kd_tree<T>::createTree(const std::vector<value_type>& objs, NodeSplitter func)
{
  size_type objs_size = objs.size();
  BBox common_bound = math::calcGroupBound(objs);

  std::vector<size_type> ids(objs_size);
  for(size_type idx = 0; idx < objs_size; ++idx)
    ids[idx] = idx;

  obj_used_.resize(1);
  obj_used_ids_.resize(1);
  obj_used_[0].resize(objs_size, 0); // 0 - false

  root_ = createTree(objs_size, common_bound, ids, func, 0);
}

// TODO: rewrite to decrease form to take off max_depth variable
template <typename T>
typename kd_tree<T>::node_type* kd_tree<T>::createTree(size_type& leafs_count,
                                                      const BBox& bbox,
                                                      std::vector<size_type>& obj_ids,
                                                      NodeSplitter func,
                                                      const size_type depth,
                                                      const size_type max_depth)
{
  node_type* tree_node = new node_type(bbox, depth);

  auto split_result = utils::splitNode(tree_node, func, objs_, obj_ids);

  if (depth < max_depth && split_result.has_value())
  {
    if (!split_result->lhs_obj_ids.empty())
      tree_node->setLeft(createTree(leafs_count, split_result->lhs_bound, split_result->lhs_obj_ids, func, depth + 1, max_depth));
    else
      tree_node->setLeft(nullptr);

    if (!split_result->rhs_obj_ids.empty())
      tree_node->setRight(createTree(leafs_count, split_result->rhs_bound, split_result->rhs_obj_ids, func, depth + 1, max_depth));
    else
      tree_node->setRight(nullptr);
  }
  else
  {
    tree_node->setData(obj_ids);
    // TODO: this is responsibility of node_type ctr. Delete it
    tree_node->setLeft(nullptr);
    tree_node->setRight(nullptr);

    ++leafs_count;
  }

  return tree_node;
}

template <typename T>
void kd_tree<T>::getLeafNodes(std::vector<node_type*>& leaf_nodes) const
{
  size_type id = 0;

  leaf_nodes.resize(size_);
  getLeafNodesImpl(leaf_nodes, root_, id);
}

template <typename T>
void kd_tree<T>::getLeafNodesImpl(std::vector<node_type*>& leaf_nodes, node_type* node, size_type& id) const
{
  if (node->getLeft() == nullptr && node->getRight() == nullptr)
  {
    leaf_nodes[id++] = node;
    return;
  }

  if (node->getLeft() != nullptr)
    getLeafNodesImpl(leaf_nodes, node->getLeft(), id);
  if (node->getRight() != nullptr)
    getLeafNodesImpl(leaf_nodes, node->getRight(), id);
}

template <typename T>
const typename kd_tree<T>::node_type* const kd_tree<T>::findNearestLeaf(const Point3f& point) const
{
  return findNearestLeafImpl(root_, point);
}

template <typename T>
const typename kd_tree<T>::node_type* const
kd_tree<T>::findNearestLeafImpl(const node_type* node, const Point3f& point) const
{
  if (node->getLeft() == nullptr && node->getRight() == nullptr)
    return node;

  if (node->getRight() == nullptr)
    return findNearestLeafImpl(node->getLeft(), point);

  if (node->getRight() == nullptr);
    return findNearestLeafImpl(node->getRight(), point);

  Point3f lhs_mid_pivot = math::calcMid(node->getLeft()->getBBox());
  Point3f rhs_mid_pivot = math::calcMid(node->getRight()->getBBox());

  if ((point - lhs_mid_pivot).squaredNorm() < (point - rhs_mid_pivot).squaredNorm())
    return findNearestLeafImpl(node->getLeft(), point);
  
  return findNearestLeafImpl(node->getRight(), point);
}

template <typename T>
void kd_tree<T>::findNearestObj(NearestInfo& info, const Point3f& point)
{
  const node_type* const leaf = findNearestLeaf(point);

  info = utils::findNearestObj(leaf, obj_used_[0], obj_used_ids_[0], objs_, point);

  if (std::fabs(info.min_dist2 - 0.0f) >= eps2 && !math::canInscribeSphereInBBox(leaf->getBBox(), point, info.min_dist))
    findNearestObjInRadius(info, point);

  size_type obj_used_ids_size = obj_used_ids_[0].size();

  for(size_type idx = 0; idx < obj_used_ids_size; ++idx)
    obj_used_[0][obj_used_ids_[0][idx]] = 0; // 0 -> false

  obj_used_ids_[0].clear();
}

template <typename T>
void kd_tree<T>::findNearestObjInRadius(NearestInfo& info, const Point3f& point)
{
  findNearestObjInRadiusImpl(info, root_, point);
}

template <typename T>
void kd_tree<T>::findNearestObjInRadiusImpl(NearestInfo& info, const node_type* node, const Point3f& point)
{
  if (math::isBBoxIntersectSphere(node->getBBox(), point, info.min_dist))
  {
    if (node->getLeft() == nullptr && node->getRight() == nullptr)
    {
      auto result = utils::findNearestObj(node, obj_used_[0], obj_used_ids_[0], objs_, point);
      if (result.min_dist < info.min_dist)
        info = *result;
      return;      
    }

    if (node->getLeft() != nullptr)
      findNearestObjInRadiusImpl(info, node->getLeft(), point);

    if (node->getRight() != nullptr)
      findNearestObjInRadiusImpl(info, node->getLeft(), point);
  }
}

template <typename T>
kd_tree<T>::~kd_tree()
{
  // TODO: decline hand memory usage
  if (root_ != nullptr)
    delete root_;
}