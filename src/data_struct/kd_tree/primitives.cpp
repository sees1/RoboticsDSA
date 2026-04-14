#include <stdexcept>
#include "primitives.hpp"

namespace study {
namespace primitives {
Triangle::Triangle(const Point3f& p1, const Point3f& p2, const Point3f& p3)
: Base(Type::TriangleType), v{p1, p2, p3}
{
  calcBBox();
}

void Triangle::setTriange(const Point3f& p1, const Point3f& p2, const Point3f& p3)
{
  v[0] = p1;
  v[1] = p2;
  v[2] = p3;
  calcBBox();
}

const BBox& Triangle::getBBox() const
{
  return bound;
}

const Point3f& Triangle::operator[](const size_type dim) const
{
  if (dim >= 3)
    throw std::runtime_error("Out of dimension!");

  return v[dim];
}

void Triangle::calcBBox()
{
  for (size_type dim = 0; dim < 3; ++dim)
  {
    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::min();
    for (size_type idx = 0; idx < 3; ++idx)
    {
      min = std::min(min, v[idx](dim));
      max = std::max(max, v[idx](dim));
    }
    bound.min(dim) = min;
    bound.max(dim) = max;
  }
}
} // namespace primitives
} // namespace study