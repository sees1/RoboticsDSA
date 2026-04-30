#include "vector_map_app/map.hpp"
#include "vector_map_app/primitives/road_arc.hpp"
#include "vector_map_app/primitives/road_straight.hpp"

#include <algorithm>

void Map::addRoad(std::shared_ptr<Road> road)
{
  graph_.merge(road->getGraph());
  roads_.push_back(road);
  refreshGraph();
}

void Map::deleteRoad(const QPointF& point)
{
  auto remove_iter = std::remove_if(roads_.begin(), roads_.end(),
    [this, &point](const auto& road)
    {
      if (road->underPoint(point)) 
      {
        auto control_vertice = road->getControlPoints();
        Vertice base_vertice_l, base_vertice_r;
        std::tie(base_vertice_l, base_vertice_r) = road->getBasePoints();
        remove_vertices_.insert(remove_vertices_.end(), control_vertice.begin(), control_vertice.end());

        // remove base vertice of deleted road
        for(auto& [u, neighbors] : graph_)
        {
          neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), base_vertice_l), neighbors.end());
          neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), base_vertice_r), neighbors.end());
        }

        graph_.erase(base_vertice_l);
        graph_.erase(base_vertice_r);

        // add roads that should be cleaned  
        roads_need_clean_.insert(roads_need_clean_.end(), intersection_[road].begin(), intersection_[road].end());

        // remove deleted road from dependency roads
        for(auto& [key_road, inter_road] : intersection_)
        {
          inter_road.erase(std::remove(inter_road.begin(), inter_road.end(), road), inter_road.end());
        }

        intersection_.erase(road);

        return true;
      }

      return false;
    }
  );

  roads_.erase(remove_iter, roads_.end());

  std::vector<std::shared_ptr<Road>> clean_roads;

  std::for_each(roads_need_clean_.begin(), roads_need_clean_.end(),
    [this, &clean_roads](auto& road)
    {
      if (intersection_.find(road) != intersection_.end())
        clean_roads.push_back(road);
    }
  );

  roads_need_clean_ = clean_roads;

  // add more intelligent check for need graph refresh
  if (!roads_need_clean_.empty())
    refreshGraph();
}

void Map::setPolyCount(int poly_count)
{
  poly_count_ = poly_count;
}

std::vector<std::shared_ptr<Road>>& Map::getRoads()
{
  return roads_;
}

std::vector<std::vector<RoadGeometrySegment>> Map::getGraphPolygons() const
{
  std::vector<std::vector<RoadGeometrySegment>> res;

  for (const auto& road : roads_)
  {
    res.push_back(road);
  }

  return res;
}

std::map<Vertice, std::vector<Vertice>> Map::getGraph() const
{
  return graph_;
}

void Map::loadRoads(std::vector<std::vector<std::tuple<Vertice, Vertice, QPolygonF, int>>>& polygons)
{
  // restore road geometry + render
  roads_.clear();
  
  size_t polygons_s = polygons.size();
  for(int i = 0; i < polygons_s; ++i)
  {
    size_t poly_s = polygons[i].size();
    
    std::vector<Vertice> all_v;
    QPolygonF res_poly;
    int type;

    for(int j = 0; j < poly_s; ++j)
    {
      Vertice lhs, rhs;
      QPolygonF poly;

      std::tie(lhs, rhs, poly, type) = polygons[i][j];

      all_v.push_back(lhs);

      size_t poly_s = poly.size();

      if (j == poly_s - 1)
        all_v.push_back(rhs);

      for(size_t k = 0; k < poly_s; ++k)
        res_poly << poly[k];
    }

    if (type == 0) // Arc
      roads_.push_back(std::make_shared<RoadArc>(res_poly, all_v));
    else if (type == 1) // Straight
      roads_.push_back(std::make_shared<RoadStraight>(res_poly, all_v));
  }
}

void Map::loadGraph(std::map<Vertice, std::vector<Vertice>>& graph)
{
  graph_ = graph;

  intersection_.clear();

  // TODO: add here processing case where more than 2 roads crossed in one vertice (maybe 3 or more)
  for (const auto& [vertice, adj_v] : graph_)
  {
    if (adj_v.size() == 4) // conect 2 roads
    {
      bool first_found = false;
      std::shared_ptr<Road> first_road = nullptr;
      std::shared_ptr<Road> second_road = nullptr;
      for (const auto& road : roads_)
      {
        std::vector<Vertice> c_v = road->getControlPoints();

        for (auto& v : c_v)
        {
          if (v == vertice) // first road found
          {
            if (!first_found)
            {
              first_road = road;
              first_found = true;
            }
            else
            {
              first_found = false;
              second_road = road;
            }
          }
        }

        if (first_road != nullptr && second_road != nullptr)
        {
          intersection_[first_road].push_back(second_road);
          intersection_[second_road].push_back(first_road);

          first_road = nullptr;
          second_road = nullptr;
        }
      }
    }
  }
}

void Map::refreshGraph()
{
  if (!remove_vertices_.empty())
  {
    std::vector<std::pair<std::shared_ptr<Road>, Vertice>> remove_from_road;

    for (auto & road : roads_need_clean_)
    {
      auto control_points = road->getControlPoints();

      // remove point from current (not from graph!) road
      std::for_each(control_points.begin(), control_points.end(),
                    [this, &remove_from_road, &road](auto& point)
                    {
                      if (std::find(remove_vertices_.begin(), remove_vertices_.end(), point) != remove_vertices_.end())
                      {
                        remove_from_road.push_back({road, point});
                        road->removeVerticeById(point.id);
                      }
                    }
                  );
    }

    roads_need_clean_.clear();
    remove_vertices_.clear();

    for(auto& [road, vertice] : remove_from_road)
    {
      for(auto& [u, neighbors] : graph_)
      {
        neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), vertice), neighbors.end());
      }

      graph_.erase(vertice);

      Vertice road_l, road_r;
      std::tie(road_l, road_r) = road->betweenIds(vertice.coord);

      graph_[road_l].push_back(road_r);
      graph_[road_r].push_back(road_l);
    }

    return;
  }

  // TODO: add here a intelligent check for roads_ emptyness
  if (roads_.size() != 0)
  {
    const auto& new_road = roads_.back();
    const auto& new_road_path = roads_.back()->getPath(QPoint(0, 0));

    std::for_each(roads_.begin(), roads_.end() - 1,
      [this, &new_road, &new_road_path](const auto& road)
      {
        const auto& road_path = road->getPath(QPoint(0, 0));

        if (road_path.intersects(new_road_path))
        {
          auto road_poly = road->getPathPolygons();
          auto new_road_poly = new_road->getPathPolygons();

          QPointF inter_point = intersectPathPoint(road_poly, new_road_poly);
          vertices_idx_++;

          Vertice new_road_l, new_road_r;
          Vertice old_road_l, old_road_r;

          std::tie(new_road_l, new_road_r) = new_road->betweenIds(inter_point);
          std::tie(old_road_l, old_road_r) = road->betweenIds(inter_point);

          auto& vec_n_l = graph_[new_road_l];
          graph_[new_road_l].erase(std::remove(vec_n_l.begin(), vec_n_l.end(), new_road_r), vec_n_l.end());
          auto& vec_n_r = graph_[new_road_r];
          graph_[new_road_r].erase(std::remove(vec_n_r.begin(), vec_n_r.end(), new_road_l), vec_n_r.end());

          auto& vec_o_l = graph_[old_road_l];
          graph_[old_road_l].erase(std::remove(vec_o_l.begin(), vec_o_l.end(), old_road_r), vec_o_l.end());
          auto& vec_o_r = graph_[old_road_r];
          graph_[old_road_r].erase(std::remove(vec_o_r.begin(), vec_o_r.end(), old_road_l), vec_o_r.end());

          Vertice temp = new_road->addVertice(inter_point, vertices_idx_);
          road->addVertice(inter_point, vertices_idx_);

          graph_[new_road_l].push_back(temp);
          graph_[new_road_r].push_back(temp);
          graph_[old_road_l].push_back(temp);
          graph_[old_road_r].push_back(temp);
          graph_[temp].push_back(new_road_l);
          graph_[temp].push_back(new_road_r);
          graph_[temp].push_back(old_road_l);
          graph_[temp].push_back(old_road_r);

          intersection_[road].push_back(new_road);
          intersection_[new_road].push_back(road);
        }
      }
    );
  }
}

QPointF Map::intersectPathPoint(const QList<QPolygonF>& lhs, const QList<QPolygonF>& rhs)
{
  // check if point lies within both segments
  auto between = [](double left_border, double right_border, double value)
  {
      return (value >= std::min(left_border, right_border) - 1e-6 &&
              value <= std::max(left_border, right_border) + 1e-6);
  };

  for (const QPolygonF& poly_lhs : lhs)
  {
    for(const QPolygonF& poly_rhs : rhs)
    {
      if (poly_rhs.intersects(poly_lhs))
      {
        for (int i = 0; i < poly_lhs.size() - 1; ++i)
        {
          QPointF p_l1 = poly_lhs[i];
          QPointF p_l2 = poly_lhs[i + 1];

          for (int j = 0; j < poly_rhs.size() - 1; ++j)
          {
            QPointF p_r1 = poly_rhs[j];
            QPointF p_r2 = poly_rhs[j + 1];

            double x1 = p_l1.x();
            double y1 = p_l1.y();
            double x2 = p_l2.x();
            double y2 = p_l2.y();
            double x3 = p_r1.x();
            double y3 = p_r1.y();
            double x4 = p_r2.x();
            double y4 = p_r2.y();

            double denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4); // vector dot product
            if (qFuzzyIsNull(denom))
              continue;

            double px = ((x1*y2 - y1*x2)*(x3-x4) - (x1-x2)*(x3*y4 - y3*x4)) / denom;
            double py = ((x1*y2 - y1*x2)*(y3-y4) - (y1-y2)*(x3*y4 - y3*x4)) / denom;

            if (between(x1, x2, px) && between(y1, y2, py) &&
                between(x3, x4, px) && between(y3, y4, py))
            {
              return QPointF(px, py);
            }
          }
        }
      }
    }
  }
}