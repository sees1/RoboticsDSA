#pragma once

#include "vector_map_app/primitives/road_geometry.hpp"

class Road : public RoadGeometry
{
public:
  using GraphType = std::map<Vertice, std::vector<Vertice>>;

public:
  Road() = delete;
  Road(std::shared_ptr<QPainterPath> view, const std::vector<std::pair<QPointF, size_t>>& points, size_t poly_count);
  Road(std::shared_ptr<QPainterPath> view, const std::vector<Vertice>& vertices, size_t poly_count);
  
  virtual ~Road() = default;

  // render path invoke
  QPainterPath getPath(const QPoint& offset);
  bool underPoint(const QPointF& offset);

protected:
  std::shared_ptr<QPainterPath> path_;
};
