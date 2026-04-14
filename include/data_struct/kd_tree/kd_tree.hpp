#include <vector>
#include <map>
#include <set>
#include <cstdlib>
#include <functional>

#include "util_structs.hpp"
#include "kd_tree_node.hpp"
#include "node_utils.hpp"
#include "math.hpp"

namespace study {
  template <typename T>
  class kd_tree final
  {
  public:
    using value_type = T;
    using node_type = kd_tree_node;
    using NodeSplitter = utils::NodeSplitter<value_type>;

  public:
    kd_tree(const std::vector<value_type>& objs, NodeSplitter func, bool multithread = false);
    kd_tree()                                                        = delete; // default ctr
    kd_tree(const kd_tree<value_type>& other)                        = delete; // copy ctr
    kd_tree(kd_tree<value_type>&& other)                             = delete; // move ctr
    kd_tree<value_type>& operator=(const kd_tree<value_type>& other) = delete; // copy assign
    kd_tree<value_type>& operator=(kd_tree<value_type>&& other)      = delete; // move assign
    ~kd_tree();

    void findNearestObj(NearestInfo& info, const Point3f& point);
    void findNearestObjInRadius(NearestInfo& info, const Point3f& point);

    const node_type* const findNearestLeaf(const Point3f& point) const;

    // TODO: transform to operator std::vector<node_type*>() and size() correspondingly
    void getLeafNodes(std::vector<node_type*>& leaf_nodes) const;
    size_type getNumLeafs() const;

  private:
    void createTree(const std::vector<value_type>& objs, NodeSplitter func);
    node_type* createTree(size_type& leafs_count,
                          const BBox& bbox,
                          std::vector<size_type>& obj_ids,
                          NodeSplitter func,
                          const size_type depth,
                          const size_type max_depth = 32);

    void getLeafNodesImpl(std::vector<node_type*>& leaf_nodes, node_type* node, size_type& id) const;
    const node_type* const findNearestLeafImpl(const node_type* node, const Point3f& point) const;
    void findNearestObjInRadiusImpl(NearestInfo& info, const node_type* node, const Point3f& point);

  private:
    bool use_multithread_;
    std::vector<std::vector<size_type>> obj_used_ids_;
    std::vector<std::vector<int>> obj_used_;

    std::vector<value_type> objs_;

    node_type* root_;
    size_type size_;
  };

#include "kd_tree_impl.hpp"
} // namespace study