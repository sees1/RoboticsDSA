#include "kd_tree_node.hpp"

namespace study {
  kd_tree_node::kd_tree_node(const BBox& bound, size_type depth)
  : bound_(bound),
    depth_(depth),
    left_(nullptr),
    right_(nullptr)
  { }

  kd_tree_node::~kd_tree_node()
  {
    if (left_ != nullptr)
      delete left_;
    if (right_ != nullptr)
      delete right_;
  }

  kd_tree_node* kd_tree_node::getLeft()
  {
    return left_;
  }

  const kd_tree_node* kd_tree_node::getLeft() const
  {
    return left_;
  }

  kd_tree_node* kd_tree_node::getRight()
  {
    return right_;
  }

  const kd_tree_node* kd_tree_node::getRight() const
  {
    return right_;
  }

  void kd_tree_node::setLeft(kd_tree_node* left)
  {
    left_ = left;
  }

  void kd_tree_node::setRight(kd_tree_node* right)
  {
    right_ = right;
  }

  void kd_tree_node::setBBox(const BBox& bound)
  {
    bound_ = bound;
  }

  void kd_tree_node::setDepth(size_type depth)
  {
    depth_ = depth;
  }

  void kd_tree_node::setData(const std::vector<size_type>& obj_ids)
  {
    // TODO: find out needed we obj_ids vector or not. if so don't clear, another case - clear
    if (obj_ids_.empty())
    {
      obj_ids_.resize(obj_ids.size());
    }
    else
    {
      obj_ids_.clear();
      obj_ids_.resize(obj_ids.size());
    }

    std::copy(obj_ids.begin(), obj_ids.end(), obj_ids_.begin());
  }

  void kd_tree_node::setData(std::vector<size_type>&& obj_ids)
  {
    obj_ids_ = std::move(obj_ids);
  }

  const std::vector<size_type>& kd_tree_node::getData() const
  {
    return obj_ids_;
  }

  const BBox& kd_tree_node::getBBox() const
  {
    return bound_;
  }

  size_type kd_tree_node::getDepth() const
  {
    return depth_;
  }

  size_type kd_tree_node::size() const
  {
    return obj_ids_.size();
  }
} //namespace study