#pragma once

#include "vector_map_app/primitives/road_base.hpp"

#include <QString>

#include <optional>

class Map;

std::ofstream& operator<<(std::ofstream& out, QPolygonF& obj);

class MapData final {
public:
  MapData() = default;
  ~MapData() = default;

  void saveTo(const QString& filename, Map* map, QTransform& map_offset, float resolution);
  void loadTo(const QString& filename, Map* map);

private:

  void scaleVertice(Vertice& v, float resolution);

  template <typename Iter, typename = std::void_t<std::enable_if_t<std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<Iter>::iterator_category>>,
                                                  std::enable_if_t<std::is_same_v<QPointF, typename std::iterator_traits<Iter>::value_type>>>>
  void scalePointRange(Iter begin, Iter end, float resolution);

  template <typename Iter, typename = std::void_t<std::enable_if_t<std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<Iter>::iterator_category>>,
                                                  std::enable_if_t<std::is_same_v<QPointF, typename std::iterator_traits<Iter>::value_type>>>>
  void translatePointRange(Iter begin, Iter end, const QTransform& map_offset);
};

template <typename Iter, typename>
void MapData::scalePointRange(Iter begin, Iter end, float resolution)
{
  std::for_each(begin, end, [&resolution](auto&& point)
  {
    point.setX(point.x() * resolution);
    point.setY(point.y() * resolution);
  });
}

template <typename Iter, typename>
void MapData::translatePointRange(Iter begin, Iter end, const QTransform& map_offset)
{
  std::for_each(begin, end, [&map_offset](auto& point) mutable
  {
    point = map_offset.map(point);
  });
}