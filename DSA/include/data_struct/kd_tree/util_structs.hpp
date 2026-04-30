#pragma once
#include <limits>

#include "common.hpp"

namespace study {
  struct NearestInfo
  {
    size_type id = 0;
    float min_dist = std::numeric_limits<float>::max();
    float min_dist2 = std::numeric_limits<float>::max();
  };

  struct SplitInfo
  {
    float pivot;
    size_type split_dim;
    size_type lhs_objs_size = 0;
    size_type rhs_objs_size = 0;
  };

  struct BBox
  {
    Point3f min, max;
  };

  struct SplitResult
  {
    std::vector<size_type> lhs_obj_ids;
    std::vector<size_type> rhs_obj_ids;
    BBox lhs_bound;
    BBox rhs_bound;
  };

} // namespace study