template <typename T>
kd_tree<T>::kd_tree(const std::vector<value_type>& objs, NodeSplitter func)
: objs_(objs),
  size_(0)
{
  size_type objs_size = objs.size();
  BBox common_bound = math::calcGroupBound(objs);

  std::vector<size_type> ids(objs_size);
  for(size_type idx = 0; idx < objs_size; ++idx)
    ids[idx] = idx;

  obj_used_.resize(objs_size, 0); // 0 - false

  root_ = std::shared_ptr<node_type>(std::createTreeImpl(objs_size, common_bound, ids, func, 0));
}

template <typename T>
typename kd_tree<T>::node_type* kd_tree<T>::createTreeImpl(size_type& leafs_count,
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
      tree_node->setLeft(createTreeImpl(leafs_count, split_result->lhs_bound, split_result->lhs_obj_ids, func, depth - 1, max_depth));

    if (!split_result->rhs_obj_ids.empty())
      tree_node->setRight(createTreeImpl(leafs_count, split_result->rhs_bound, split_result->rhs_obj_ids, func, depth + 1, max_depth));
  }
  else
  {
    // leaf case
    tree_node->setData(obj_ids);
    ++leafs_count;
  }

  return tree_node;
}

template <typename T>
kd_node_info kd_tree<T>::findNearestLeaf(const Point3f& point) const
{
  std::shared_ptr<node_type> node = root_;

  while (node->getLeft() != nullptr && node->getRight() != nullptr)
  {
    if (node->getRight() == nullptr)
    {
      node = node->getRight();
      continue;
    }

    if (node->getLeft() == nullptr)
    {
      node = node->getLeft();
      continue;
    }

    Point3f lhs_mid_pivot = math::calcMid(node->getLeft()->getBBox());
    Point3f rhs_mid_pivot = math::calcMid(node->getRight()->getBBox());

    if ((point - lhs_mid_pivot).squaredNorm() < (point - rhs_mid_pivot).squaredNorm())
      node = node->getLeft();
    else
      node = node->getRight();
  }

  return kd_node_info(*node);
}

template <typename T>
NearestInfo kd_tree<T>::findNearestObj(const Point3f& point)
{
  kd_node_info leaf_info = findNearestLeaf(point);

  auto info = utils::findNearestObj(leaf_info, obj_used_, obj_used_ids_, objs_, point);

  if (std::fabs(info.min_dist2 - 0.0f) >= eps2 && !math::canInscribeSphereInBBox(leaf_info.bound(), point, info.min_dist))
    findNearestObjInRadius(*info, point);

  size_type obj_used_ids_size = obj_used_ids_.size();

  for(size_type idx = 0; idx < obj_used_ids_size; ++idx)
    obj_used_[obj_used_ids_[idx]] = 0; // 0 -> false

  obj_used_ids_.clear();

  return *info;
}

template <typename T>
NearestInfo kd_tree<T>::findNearestObjInRadius(const Point3f& point)
{
  NearestInfo info;
  findNearestObjInRadiusImpl(info, root_, point);
  return info;
}

template <typename T>
size_type kd_tree<T>::size() const
{
  return size_;
}

template <typename T>
void kd_tree<T>::findNearestObjInRadiusImpl(NearestInfo& info, std::shared_ptr<node_type> node, const Point3f& point)
{
  if (math::isBBoxIntersectSphere(node->getBBox(), point, info.min_dist))
  {
    if (node->getLeft() == nullptr && node->getRight() == nullptr)
    {
      auto result = utils::findNearestObj(node, obj_used_, obj_used_ids_, objs_, point);
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