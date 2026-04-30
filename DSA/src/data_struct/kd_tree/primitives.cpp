#include <stdexcept>
#include "primitives.hpp"

namespace study {
namespace primitives {
Triangle::Triangle(const Point3f& p1, const Point3f& p2, const Point3f& p3)
: Polygon<3>({p1, p2, p3}, Type::TriangleType)
{ }

Box::Box(const Point3f& p1, const Point3f& p2, const Point3f& p3, const Point3f& p4)
: Polygon<4>({p1, p2, p3, p4}, Type::BoxType)
{ }

} // namespace primitives
} // namespace study