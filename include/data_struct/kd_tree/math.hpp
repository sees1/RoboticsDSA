#pragma once
#include "util_structs.hpp"
#include "primitives.hpp"

#include <cmath>

namespace study {
namespace math {
  using namespace study::primitives;

  Point3f calcMid(const BBox& bound)
  {
    return Point3f(0.5f * (bound.min + bound.max));
  }

  bool canInscribeSphereInBBox(const BBox& bound, const Point3f& center, float radius)
  {
    return (bound.min(0) < center(0) - radius) && (center(0) + radius < bound.max(0)) &&
           (bound.min(1) < center(1) - radius) && (center(1) + radius < bound.max(1)) &&
           (bound.min(2) < center(2) - radius) && (center(2) + radius < bound.max(2));
  }

  bool isBBoxIntersectSphere(const BBox& bound, const Point3f& center, float radius)
  {
    Point3f mid = calcMid(bound);
    Eigen::Vector3f distance = center - mid;
    Eigen::Vector3f half_bbox_length = 0.5f * (bound.min - bound.max);
    
    // heuristic1: distance between center of bbox and circle center too big (enought only by one axis)
    if (half_bbox_length(0) + radius < distance(0) || half_bbox_length(1) + radius < distance(1) || half_bbox_length(2) + radius < distance(2))
      return false;

    // heuristic2: center inside bbox
    if (distance(0) <= half_bbox_length(0) || distance(1) <= half_bbox_length(1) || distance(2) <= half_bbox_length(2))
      return true;

    // check if dist from bbox corner to circle center less than radius^2
    Eigen::Vector3f distFromCorner = distance - half_bbox_length;
    
    return distFromCorner.squaredNorm() <= std::pow(radius, 2);
  }

  template <typename T>
  BBox calcGroupBound(const std::vector<T>& objs)
  {
    // TODO: add concept for all T that should have min/max Point3f
    // TODO: add concept for all T that should have getBBox() func
    size_t sz = objs.size();
    BBox common_bound;
    for (size_t dim = 0; dim < 3; ++dim)
    {
      float min = std::numeric_limits<float>::max();
      float max = std::numeric_limits<float>::min();
      for (int idx = 0; idx < sz; ++idx)
      {
        const BBox& obj = objs[idx].getBBox();
        min = std::min(min, obj.min(dim));
        max = std::max(max, obj.max(dim));
      }
      common_bound.min(dim) = min;
      common_bound.max(dim) = max;
    }
    return common_bound;
  }

  float distToSeg2(const Point3f& A, const Point3f& B, const Point3f& point)
  {
    Point3f AB = B - A;
    float seg_len2 = AB.squaredNorm();
    if (std::fabs(seg_len2 - 0.0f) < eps2)
      return (point - A).squaredNorm();

    float sign_value = std::fmin(1.0f, (point - A).dot(AB) / seg_len2);
    float ab_coeff = std::fmax(0.0f, sign_value);

    Point3f proj = A + ab_coeff * AB;
    return (proj - point).squaredNorm();
  }

  float distToSeg(const Point3f& A, const Point3f& B, const Point3f& point)
  {
    return std::sqrt(distToSeg2(A, B, point));
  }

  // primary
  template <typename Primitive>
  float dist2(const Primitive& obj, const Point3f& point);

  template <>
  float dist2<Triangle>(const Triangle& obj, const Point3f& point)
  {
    Point3f E0 = obj[1] - obj[0];
    Point3f E1 = obj[2] - obj[0];
    Point3f v  = obj[0] - point;

    float a = E0.dot(E0);
    float b = E0.dot(E1);
    float c = E1.dot(E1);
    float d = E0.dot(v);
    float e = E1.dot(v);

    float det = a * c - b * b;
    float s = (b * e - c * d) / det;
    float t = (b * d - a * e) / det;

    if (s >= 0.0f && t >= 0.0f && (s + t) <= 1.0f)
      return (obj[0] + s * E0 + t * E1 - point).squaredNorm();

    return std::min({distToSeg2(obj[0], obj[1], point),
                     distToSeg2(obj[1], obj[2], point),
                     distToSeg2(obj[2], obj[0], point)});
  }

  template <typename Primitive>
  float dist(const Primitive& obj, const Point3f& point)
  {
    return std::sqrt(dist2(obj, point));
  }

} // namespace math
} // namespace study