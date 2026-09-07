#include "util_structs.hpp"

namespace study {
namespace primitives {
  enum Type { TriangleType = 0, BoxType = 1 };

  // TODO: add 2D primitives
  class PolygonBase
  {
    public:
      PolygonBase(Type type) : type_(type) { }
      virtual ~PolygonBase() = default;

      Type getType() const { return type_; }

      virtual const BBox& getBBox() const = 0;
      virtual const Point3f& operator[](const size_type dim) const = 0;

    private:
    const Type type_;
  };

  template <size_type N>
  class Polygon : public PolygonBase {
  public:
    static constexpr size_type dimension_size = N;
    
  public:
    Polygon(const std::array<Point3f, N>& cont, Type type) : PolygonBase(type), v(cont) { calcBBox(); }
    virtual ~Polygon() = default;

    const BBox& getBBox() const override { return bound; }
    const Point3f& operator[](const size_type idx) const override
    {
      if (idx >= N)
        throw std::runtime_error("Out of dimension!");
    
      return v[idx];
    }

  private:
    void calcBBox()
    {
      for (size_type dim = 0; dim < dimension_size; ++dim)
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

  protected:
    BBox bound;
    std::array<Point3f, N> v;
  };

  class Triangle final : public Polygon<3> {
  public:
    Triangle(const Point3f& p1, const Point3f& p2, const Point3f& p3);
  };

  class Box final : public Polygon<4> {
  public:
    Box(const Point3f& p1, const Point3f& p2, const Point3f& p3, const Point3f& p4);
  };
} // namespace primitives
} // namespace study