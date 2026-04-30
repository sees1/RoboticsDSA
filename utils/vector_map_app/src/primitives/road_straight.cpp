#include "vector_map_app/primitives/road_straight.hpp"

RoadStraight::RoadStraight(const std::vector<std::pair<QPointF, size_t>>& points, size_t poly_count) 
: Road(makePath(points), points, poly_count)
{ }

RoadStraight::RoadStraight(const QPolygonF& polygon, const std::vector<Vertice>& vertices, size_t poly_count)
: Road(makePath(polygon), vertices, poly_count)
{ }

std::shared_ptr<QPainterPath> RoadStraight::makePath(const std::vector<std::pair<QPointF, size_t>>& points)
{
  path_ = std::make_shared<QPainterPath>(points[0].first);
  path_->lineTo(points[1].first);
}

std::shared_ptr<QPainterPath> RoadStraight::makePath(const QPolygonF& polygon)
{
  path_ = std::make_shared<QPainterPath>();
  path_->addPolygon(polygon);

  return path_;
}