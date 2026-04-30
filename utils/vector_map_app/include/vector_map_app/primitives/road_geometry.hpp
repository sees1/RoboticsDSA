#pragma once

#include <memory>
#include <tuple>
#include <map>
#include <limits>
#include <algorithm>

#include <QtWidgets>

struct Vertice
{
  int id;
  QPointF coord;

  bool operator<(const Vertice& other) const { return id < other.id; }
  bool operator==(const Vertice& other) const { return id == other.id; }
};

struct RoadGeometrySegment
{
  Vertice lhs_; // vertice ordering by ppoint pos
  Vertice rhs_; // vertice ordering by ppoint pos

  QPolygonF part;
};

class RoadGeometry
{
public:
  RoadGeometry() = delete;
  RoadGeometry(std::shared_ptr<QPainterPath> view, const std::vector<Vertice>& vertices, size_t poly_count);
  RoadGeometry(std::shared_ptr<QPainterPath> view, const std::vector<std::pair<QPointF, size_t>>& points, size_t poly_count);

  virtual ~RoadGeometry() = default;

  Vertice addVertice(const QPointF& point, int id);
  bool removeVerticeById(int id);
  RoadGeometrySegment betweenIds(const QPointF& point);
  RoadGeometrySegment betweenIds(int first, int second);
  std::vector<Vertice> getControlPoints(); // vertice ordering by ppoint pos
  std::tuple<Vertice, Vertice> getBasePoints(); // vertice ordering by ppoint pos
  std::map<Vertice, std::vector<Vertice>> getGraph();

  operator std::vector<RoadGeometrySegment> ();

private:
  size_t findClosestPoint(const QPointF& target);
  void subdividePath(std::shared_ptr<QPainterPath> view, size_t poly_count);

protected:
  std::map<int, Vertice> ppoint_to_vertice_; // conversion for ppoint (sorted by distance from map begin)
  std::map<Vertice, int> vertice_to_ppoint_;

  QPolygonF path_;
};