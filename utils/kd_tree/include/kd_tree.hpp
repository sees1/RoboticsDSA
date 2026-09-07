#include <vector>
#include <map>
#include <set>
#include <cstdlib>
#include <functional>
#include <memory>

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
    using node_type = detail::kd_tree_node;
    using NodeSplitter = utils::NodeSplitter<value_type>;

  public:
    // TODO: add multithread version of kd-tree
    kd_tree(const std::vector<value_type>& objs, NodeSplitter func);
    kd_tree()                                                        = delete; // default ctr
    kd_tree(const kd_tree<value_type>& other)                        = delete; // copy ctr
    kd_tree(kd_tree<value_type>&& other)                             = delete; // move ctr
    kd_tree<value_type>& operator=(const kd_tree<value_type>& other) = delete; // copy assign
    kd_tree<value_type>& operator=(kd_tree<value_type>&& other)      = delete; // move assign
    ~kd_tree() = default;

    NearestInfo findNearestObj(const Point3f& point);
    NearestInfo findNearestObjInRadius(const Point3f& point);
    kd_node_info findNearestLeaf(const Point3f& point) const;

    size_type size() const;

  private:
    // recursive function
    node_type* createTreeImpl(size_type& leafs_count,
                              const BBox& bbox,
                              const std::vector<size_type>& obj_ids,
                              NodeSplitter func,
                              const size_type depth,
                              const size_type max_depth = 32);
    void findNearestObjInRadiusImpl(NearestInfo& info, std::shared_ptr<node_type> node, const Point3f& point);

  private:
    bool use_multithread_;
    std::vector<size_type> obj_used_ids_;
    std::vector<int> obj_used_;

    std::vector<value_type> objs_;

    std::shared_ptr<node_type> root_;
    size_type size_;
  };

#include "kd_tree_impl.hpp"
} // namespace study