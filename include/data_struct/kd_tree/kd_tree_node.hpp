#pragma once
#include <algorithm>

#include "util_structs.hpp"

namespace study {

  // proxy for output node info
  class kd_node_info {
  public:
    kd_node_info(const kd_tree_node& node) : ref(node) { }

    const BBox& bound() const { return ref.getBBox(); }
    size_type depth() const { return ref.getDepth(); }
    size_type id(size_type idx) { return ref.getId(idx); }
    const std::vector<size_type>& ids() const { return ref.getData(); }

  private:
    const kd_tree_node& ref;
  };

  class kd_tree_node final {
  public:
    // TODO: move 5 rule responsibility to buff object inside (memory manager)
    kd_tree_node(const BBox& bound, size_type depth);
    ~kd_tree_node();

    // WARN: can make obj uncosistible
    kd_tree_node* getLeft();
    kd_tree_node* getRight();
    const kd_tree_node* getLeft() const;
    const kd_tree_node* getRight() const;
    void setLeft(kd_tree_node* left);
    void setRight(kd_tree_node* right);

    void setBBox(const BBox& bound);
    void setDepth(size_type depth);
    void setData(const std::vector<size_type>& obj_ids);
    void setData(std::vector<size_type>&& obj_ids);

    size_type getId(size_type idx) const;
    const std::vector<size_type>& getData() const;
    const BBox& getBBox() const;
    size_type getDepth() const;

    size_type size() const;

    // explicit only for block implicit conversion from node to view-only(proxy) object
    // don't use kd_node_info object after kd_tree generative object! Reference can dungle! 
    explicit operator kd_node_info() const { return kd_node_info(*this); }
  private:
    BBox bound_;
    size_type depth_;
    std::vector<size_type> obj_ids_;
    kd_tree_node* left_;
    kd_tree_node* right_;
  };
} // namespace study