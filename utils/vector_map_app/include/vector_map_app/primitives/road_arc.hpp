#pragma once

#include "vector_map_app/primitives/road_base.hpp"

class RoadArc final : public Road
{
public:
  using GraphType = Road::GraphType;
  static constexpr int points_count = 3;

public:
  RoadArc() = delete;
  RoadArc(const std::vector<std::pair<QPointF, size_t>>& points, size_t poly_count);
  RoadArc(const QPolygonF& polygon, const std::vector<Vertice>& vertices, size_t poly_count);

private:
  static std::shared_ptr<QPainterPath> makePath(const std::vector<std::pair<QPointF, size_t>>& points);
  static std::shared_ptr<QPainterPath> makePath(const QPolygonF& polygon);
};