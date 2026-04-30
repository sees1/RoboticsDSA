#include "vector_map_app/primitives/road_base.hpp"

Road::Road(std::shared_ptr<QPainterPath> view, const std::vector<std::pair<QPointF, size_t>>& points, size_t poly_count)
: RoadGeometry(view, points, poly_count),
  path_(view)
{ }

Road::Road(std::shared_ptr<QPainterPath> view, const std::vector<Vertice>& vertices, size_t poly_count)
: RoadGeometry(view, vertices, poly_count),
  path_(view)
{ }

QPainterPath Road::getPath(const QPoint& offset)
{
  return path_->translated(offset);
}

bool Road::underPoint(const QPointF& global_mouse_pose)
{
  if (path_ == nullptr)
    return false;

  QPainterPathStroker stroker;
  stroker.setWidth(15);

  QPainterPath stroke_path = stroker.createStroke(*path_);
  bool fl = stroke_path.contains(global_mouse_pose);

  return fl;
}