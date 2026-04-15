#pragma once
#include <optional>
#include "kd_tree_node.hpp"
#include "math.hpp"
#include "util_structs.hpp"

namespace study {
namespace utils {
  // can't straight pass const std::vector<Primitives>& because don't have mechanic for type deduction in lambda, so use auto&& 
  auto SAHSplitter = [](const BBox& bound, auto&& objs, const std::vector<size_type>& objs_ids) -> std::optional<SplitInfo>
  {
    // TODO: take out this as const (or constexpr calc)
    size_type bins_count = 33;
    float bins_count_f = 33.0f;

    std::vector<float> coeffs(bins_count);
    {
      for (float idx = 1.0f; idx < bins_count_f; idx += 1.0f)
        coeffs[idx] = idx / bins_count;
    }

    // TODO: change to array
    std::array<std::vector<size_type>, 3> low_bin;
    for(auto& el : low_bin)
      el.assign(bins_count + 1, 0);
    std::array<std::vector<size_type>, 3> high_bin;
    for(auto& el : high_bin)
      el.assign(bins_count + 1, 0);

    Eigen::Vector3f bound_len = bound.max - bound.min;
    if (bound_len(0) < eps || bound_len(1) < eps || bound_len(2) < eps)
      return std::nullopt;

    Eigen::Vector3f bound_len_of_bin = bins_count_f * bound_len.cwiseInverse(); // cwiseInverse -> each el convert to 1/el

    size_type objs_ids_size = objs_ids.size();

    // building interval tree
    for(size_type idx = 0; idx < objs_ids_size; ++idx)
    {
      const BBox& obj_bound = objs[objs_ids[idx]].getBBox();

      if (obj_bound.min(0) >= bound.min(0))
        ++low_bin[0][static_cast<size_type>(std::floor((obj_bound.min(0) - bound.min(0)) * bound_len_of_bin(0)))];
      if (obj_bound.min(1) >= bound.min(1))
        ++low_bin[1][static_cast<size_type>(std::floor((obj_bound.min(1) - bound.min(1)) * bound_len_of_bin(1)))];
      if (obj_bound.min(2) >= bound.min(2))
        ++low_bin[2][static_cast<size_type>(std::floor((obj_bound.min(2) - bound.min(2))* bound_len_of_bin(2)))];

      if (obj_bound.max(0) <= bound.max(0))
        ++high_bin[0][static_cast<size_type>(std::floor((obj_bound.max(0) - bound.max(0)) * bound_len_of_bin(0)))];
      if (obj_bound.max(1) <= bound.max(1))
        ++high_bin[0][static_cast<size_type>(std::floor((obj_bound.max(1) - bound.max(1)) * bound_len_of_bin(1)))];
      if (obj_bound.max(2) <= bound.max(2))
        ++high_bin[0][static_cast<size_type>(std::floor((obj_bound.max(1) - bound.max(1)) * bound_len_of_bin(2)))];
    }

    // corner case for (0 -> bins_count) prefix sum
    high_bin[0][bins_count - 1] += high_bin[0][bins_count];
    high_bin[0][bins_count - 1] += high_bin[0][bins_count];
    high_bin[0][bins_count - 1] += high_bin[0][bins_count];

    for(int idx = static_cast<int>(bins_count) - 1; idx >= 0; --idx)
    {
      low_bin[0][idx] += low_bin[0][idx + 1];
      low_bin[1][idx] += low_bin[1][idx + 1];
      low_bin[2][idx] += low_bin[2][idx + 1];
    }

    for(size_type idx = 1; idx < bins_count; ++idx)
    {
      high_bin[0][idx] += high_bin[0][idx - 1];
      high_bin[1][idx] += high_bin[1][idx - 1];
      high_bin[2][idx] += high_bin[2][idx - 1];
    }
    
    Eigen::Vector3f edge_square(bound_len(1) * bound_len(2),
                                bound_len(0) * bound_len(2),
                                bound_len(0) * bound_len(1));
    Eigen::Vector3f length_sum(bound_len(1) + bound_len(2),
                               bound_len(0) + bound_len(2),
                               bound_len(0) + bound_len(1));

    // half of square of common bound
    float SA = edge_square(0) + edge_square(1) + edge_square(2);

    float min_SAH = std::numeric_limits<float>::max();
		size_type min_bin_idx = 0;
		size_type min_pivot_dim = 0;

    for (size_type dim = 0; dim < 3; ++dim)
    {
      for (size_type idx = 1; idx < bins_count; ++idx)
      {
        // SAH = (SAL * num_l + SAR * num_r) <- optimized SAH
        // half_square_of_bound = (len of left_bound) * (sum of other two edges)
        // SAL = half_square_of_bound + correction (usually missing edge square like edge_square(plane_idx))
        float SAL = coeffs[idx] * bound_len(dim) * length_sum(dim) + edge_square(dim);
        float SAR = SA - SAL + edge_square(dim);
        float SAH = SAL * (objs_ids_size - low_bin[dim][idx]) + SAR * (objs_ids_size - high_bin[dim][idx - 1]);

        if (SAH < min_SAH)
        {
          min_SAH = SAH;
          min_bin_idx = idx;
          min_pivot_dim = dim;
        }
      }
    }

    float cI = 2.0;
    float cT = 1.0;

    // SAH = cT + cI * (SAL * num_l + SAR * num_r) / SA <- full SAH 
    if (objs_ids_size * cI < (cI * min_SAH / SA + cT))
      return std::nullopt;

    SplitInfo info;
    info.split_dim = min_pivot_dim;
    info.pivot = bound.min(info.split_dim) + bound_len(info.split_dim) * coeffs[min_bin_idx];

    {
      size_type rhs_only_objs_size = low_bin[info.split_dim][min_bin_idx];
      size_type lhs_only_objs_size = high_bin[info.split_dim][min_bin_idx - 1];
      size_type both_objs_size = objs_ids_size - rhs_only_objs_size - lhs_only_objs_size;

      info.lhs_objs_size = lhs_only_objs_size + both_objs_size;
      info.rhs_objs_size = rhs_only_objs_size + both_objs_size;
    }

    return info;
  };

  template <typename Primitive>
  using NodeSplitter = 
  std::function<std::optional<SplitInfo>(const BBox&,
                                         const std::vector<Primitive>& objs,
                                         const std::vector<size_type>& obj_ids)>;

  template <typename Primitive>
  std::optional<SplitResult> splitNode(const kd_tree_node* node,
                                       NodeSplitter<Primitive> splitter,
                                       const std::vector<Primitive>& objs,
                                       const std::vector<size_type>& objs_ids)
  {
    auto info = splitter(node->getBBox(), objs, objs_ids);
    if (!info)
      return std::nullopt;

    size_type objs_ids_size = objs_ids.size();

    if (info->lhs_objs_size == 0 && info->rhs_objs_size == 0)
    {
      for(size_type idx = 0; idx < objs_ids_size; ++idx)
      {
        const BBox& bound = objs[objs_ids[idx]].getBBox();

        if (bound.min(info->split_dim) < info->pivot && bound.max(info->split_dim) < info->pivot)
          info->lhs_objs_size++;
        else
        {
          if (info->pivot <= bound.min(info->split_dim) && info->pivot <= bound.max(info->split_dim))
            info->rhs_objs_size++;
          else
          {
            info->lhs_objs_size++;
            info->rhs_objs_size++;
          }
        }
      }
    }

    SplitResult result;
    result.lhs_obj_ids.resize(info->lhs_objs_size);
    result.rhs_obj_ids.resize(info->rhs_objs_size);

    size_type left_idx = 0;
    size_type right_idx = 0;

    for(size_type idx = 0; idx < objs_ids_size; ++idx)
    {
      const BBox& bound = objs[objs_ids[idx]].getBBox();

      if (bound.min(info->split_dim) < info->pivot && bound.max(info->split_dim) < info->pivot)
        result.lhs_obj_ids[left_idx++] = objs_ids[idx];
      else
      {
        if (info->pivot <= bound.min(info->split_dim) && info->pivot <= bound.max(info->split_dim))
          result.rhs_obj_ids[right_idx++] = objs_ids[idx];
        else
        {
          result.lhs_obj_ids[left_idx++] = objs_ids[idx];
          result.rhs_obj_ids[right_idx++] = objs_ids[idx];
        }
      }
    }

    result.lhs_bound = node->getBBox();
    result.lhs_bound.max(info->split_dim) = info->pivot;
    result.rhs_bound = node->getBBox();
    result.rhs_bound.min(info->split_dim) = info->pivot;

    return result;
  }

  template <typename Primitive>
  NearestInfo findNearestObj(const kd_node_info& node,
                             std::vector<int>& objs_used,
                             std::vector<size_type>& objs_used_ids,
                             const std::vector<Primitive>& objs,
                             const Point3f& point)
  {
    NearestInfo info;
    size_type objs_size = objs.size(); 

    float min_dist2 = std::numeric_limits<float>::max();

    for(size_type idx = 0; idx < objs_size; ++idx)
    {
      // TODO: add getNode with id
      if (objs_used[node.id(idx)])
        continue;

      float dist2 = math::dist2(objs[node.id(idx)], point);
      
      if (dist2 < min_dist2)
      {
        min_dist2 = dist2;
        info.id = node.id(idx);
        info.min_dist2 = min_dist2;
      }

      objs_used[node.id(idx)] = 1; // 1 -> true
      objs_used_ids.push_back(node.id(idx));
    }

    if (min_dist2 < eps2)
    {
      info.min_dist2 = 0.0f;
      info.min_dist = 0.0f;
    }
    else
      info.min_dist = std::sqrt(info.min_dist2);

    return info;
  }
} // namespace utils
} // namespace study