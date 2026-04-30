#include "vector_map_app/primitives/road_geometry.hpp"

// TODO: think here, maybe better to pass a range (begin, end) instead vertice cont?
RoadGeometry::RoadGeometry(std::shared_ptr<QPainterPath> view, const std::vector<Vertice>& vertices, size_t poly_count)
{
  subdividePath(view, poly_count);

  size_t v_last_idx = vertices.size() - 1;

  ppoint_to_vertice_[0] = vertices.at(0);
  ppoint_to_vertice_[poly_count] = vertices.at(v_last_idx);

  vertice_to_ppoint_[vertices.at(0)] = 0;
  vertice_to_ppoint_[vertices.at(v_last_idx)] = poly_count;

  for (size_t i = 1; i < v_last_idx; ++i)
    addVertice(vertices.at(i).coord, vertices.at(i).id);
}

RoadGeometry::RoadGeometry(std::shared_ptr<QPainterPath> view, const std::vector<std::pair<QPointF, size_t>>& points, size_t poly_count)
{
  subdividePath(view, poly_count);

  size_t v_last_idx = vertices.size() - 1;

  ppoint_to_vertice_[0] = Vertice{points[0].first, points[0].second};
  ppoint_to_vertice_[poly_count] = Vertice{points[v_last_idx].first, points[v_last_idx].second};

  vertice_to_ppoint_[ppoint_to_vertice_[0]] = 0;
  vertice_to_ppoint_[ppoint_to_vertice_[poly_count]] = poly_count;

  for (size_t i = 1; i < v_last_idx; ++i)
    addVertice(vertices[i].first, vertices[i].second);
}

Vertice RoadGeometry::addVertice(const QPointF& point, int id)
{
  size_t ppoint = findClosestPoint(point);

  ppoint_to_vertice_[ppoint] = Vertice{id, point};
  vertice_to_ppoint_[ppoint_to_vertice_[ppoint]] = ppoint;

  return ppoint_to_vertice_[ppoint];
}

bool RoadGeometry::removeVerticeById(int id)
{
  auto needed_v_iter = std::find_if(vertice_to_ppoint_.begin(), vertice_to_ppoint_.end(),
    [this, &id](auto& elem)
    {
      return id == elem.first.id;
    }
  );

  if (needed_v_iter == vertice_to_ppoint_.end())
    return false;
  else
  {
    ppoint_to_vertice_.erase(needed_v_iter->second);
    vertice_to_ppoint_.erase(needed_v_iter);
    return true;
  }
}

RoadGeometrySegment RoadGeometry::betweenIds(const QPointF& point)
{
  int ppoint = findClosestPoint(point);

  auto lower_v = ppoint_to_vertice_.lower_bound(ppoint);
  lower_v--; // get first less than ppoint vertice
  auto upper_v = ppoint_to_vertice_.upper_bound(ppoint);

  return betweenIds((*lower_v).second.id, (*upper_v).second.id)
}

RoadGeometrySegment RoadGeometry::betweenIds(int first, int second)
{
  RoadGeometrySegment res;

  if (path_.empty())
    return res;

  auto first_v_iter = std::find_if(vertice_to_ppoint_.begin(), vertice_to_ppoint_.end(), [first](auto&& v_to_p)
  {
    return v_to_p.first.id == first;
  });

  if (first_v_iter == vertice_to_ppoint_.end())
    return res;

  auto second_v_iter = std::find_if(vertice_to_ppoint_.begin(), vertice_to_ppoint_.end(), [second](auto&& v_to_p)
  {
    return v_to_p.first.id == second;
  });

  if (second_v_iter == vertice_to_ppoint_.end())
    return res;

  int first_ppoint = first_v_iter->second;
  int second_ppoint = second_v_iter->second;

  if (first_ppoint == second_ppoint)
    return res;

  auto path_begin = path_.begin() + first_ppoint;
  auto path_end = path_.begin() + second_ppoint;

  std::copy(path_begin, path_end, std::back_inserter(res.part_));

  res.lhs_ = first_v_iter->first;
  res.rhs_ = second_v_iter->first;

  return res;
}

std::vector<Vertice> RoadGeometry::getControlPoints()
{
  // TODO: swap map for vertice_to_ppoint to vector (we haven't performance glow up if we use map)
  // veritce is sorted by id!!!
  auto start = vertice_to_ppoint_.begin();
  start++;
  start++;

  std::vector<Vertice> res;
  for(start; start != vertice_to_ppoint_.end(); ++start)
  {
    res.push_back((*start).first);
  }

  return res;
}

std::tuple<Vertice, Vertice> RoadGeometry::getBasePoints()
{
  // veritce is sorted by id!!!
  auto left = vertice_to_ppoint_.begin();
  auto right = left;
  right++;
  return std::make_tuple((*left).first, (*(right)).first);
}

std::map<Vertice, std::vector<Vertice>> RoadGeometry::getGraph()
{
  std::map<Vertice, std::vector<Vertice>> res;

  auto left = vertice_to_ppoint_.begin();
  auto right = vertice_to_ppoint_.begin();
  right++;

  if (right == vertice_to_ppoint_.end())
  {
    res[left->first] = std::vector<Vertice>{right->first};
    res[right->first] = std::vector<Vertice>{left->first};

    return res;
  }

  while(right != vertice_to_ppoint_.end())
  {
    res[left->first].push_back(right->first);
    res[right->first].push_back(left->first);

    left = right;
    right++;
  }

  return res;
}

operator RoadGeometry::std::vector<RoadGeometrySegment> ()
{
  std::vector<RoadGeometrySegment> res;
  res.resize(ppoint_to_vertice_.size() - 1);
  auto rhs_iter = ppoint_to_vertice_.begin();
  std::advance(rhs_iter, 1);
  auto lhs_iter = ppoint_to_vertice_.begin();

  while (rhs_iter != ppoint_to_vertice_.end())
  {
    res.push_back(betweenIds(lhs_iter->second.id, rhs_iter->second.id));
    lhs_iter = rhs_iter;
    std::advance(rhs_iter, 1);
  }

  return res;
}

size_t RoadGeometry::findClosestPoint(const QPointF& target)
{
  auto pp_iter = std::max_element(path_.begin(), path_.end(), [&target](auto&& lhs_point, auto&& rhs_point)
  {
    float lhs_dx = lhs_point.x() - target.x();
    float lhs_dy = lhs_point.y() - target.y();
    float lhs_dist = lhs_dx + lhs_dy;

    float rhs_dx = rhs_point.x() - target.x();
    float rhs_dy = rhs_point.y() - target.y();
    float rhs_dist = rhs_dx + rhs_dy; 

    return lhs_dist < rhs_dist;
  });

  // TODO: maybe add auto as return type?
  return static_cast<size_t>(std::distance(path_.begin(), pp_iter));
}

void RoadGeometry::subdividePath(std::shared_ptr<QPainterPath> view, size_t poly_count)
{
  path_.clear();
  path_polygons_->reserve(poly_count);

  qreal step = view->length() / poly_count;
  qreal cur_len = 0;
  QPointF cur_point;
  path << view->pointAtPercent(0.0);

  for(size_t i = 1; i <= poly_count; ++i)
  {
    cur_len += step;
    path_ << view->pointAtPercent(view->percentAtLength(cur_len));
  }
}