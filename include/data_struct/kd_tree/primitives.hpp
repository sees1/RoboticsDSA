#include "util_structs.hpp"

namespace study {
namespace primitives {
  enum Type { TriangleType = 0 };

  // TODO: rule of 5 here
  class Base {
  public:
    Base(Type type) : type_(type) { }
    virtual ~Base() { }

    Type getType() const { return type_; }
    virtual const BBox& getBBox() const = 0;

  private:
    virtual void calcBBox() = 0; // TODO: add common algo

  private:
    const Type type_;
  };

  // TODO: add geometry traits
  class Triangle final : public Base {
  public:
    Triangle() : Base(Type::TriangleType) { }
    Triangle(const Point3f& p1, const Point3f& p2, const Point3f& p3);

    void setTriange(const Point3f& p1, const Point3f& p2, const Point3f& p3);

    const BBox& getBBox() const override;
    const Point3f& operator[](const size_type dim) const; // TODO: maybe add this method as a part of interface?
  
  private:
    void calcBBox() override;

  private:
    BBox bound;
    std::array<Point3f, 3> v;
  };
} // namespace primitives
} // namespace study