#pragma once
#include <algorithm>

#include "util_structs.hpp"

namespace study {

  // proxy for output node info
  class kd_node_info {
  public:
    kd_node_info(const kd_tree_node* const node) : ptr(node) { }

    const BBox& bound() const { return ptr->getBBox(); }
    size_type depth() const { return ptr->getDepth(); }
    const std::vector<size_type>& ids() const { return ptr->getData(); }

  private:
    const kd_tree_node* const ptr;
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

    const std::vector<size_type>& getData() const;
    const BBox& getBBox() const;
    size_type getDepth() const;

    size_type size() const;

  private:
    BBox bound_;
    size_type depth_;
    std::vector<size_type> obj_ids_;
    kd_tree_node* left_;
    kd_tree_node* right_;
  };
} // namespace study