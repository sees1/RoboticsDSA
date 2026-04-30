#pragma once

#include <vector>
#include <stack>

#include <QtWidgets>

#include "vector_map_app/map.hpp"
#include "vector_map_app/map_data.hpp"

enum class Mode {
  IDLE = 0,
  CREATE_ARC_ROAD,
  CREATE_STRAIGHT_ROAD,
  DELETE_ROAD,
  MOVE_CAMERA
};

class RoadBuilder {
public:
  template <typename RoadType, typename = std::void_t<decltype(RoadType::point_count)>>
  std::shared_ptr<Road> getRoad(const Mode& m)
  {
    if (collector.size() == RoadType::Traits::points_count)
    {
      bool b = std::all_of(collector.begin(), collector.end(), [&m](auto&& pair)
      {
        return m == pair->second;
      });

      clear_fl = true;

      if (b)
        return std::make_shared<RoadType>(container);
      else
        return nullptr;
    }
    else if (collector.size() > RoadType::Traits::point_count)
    {
      clear_fl = true;
    }
    else
      return nullptr;
  }

  void setNextPoint(const QPointF& point);

private:
  bool clear_fl = false;
  size_t vertices_idx = 0;

  std::vector<std::pair<QPointF, size_t>> container;
  std::vector<std::pair<QPointF, Mode>> collector;
};

class MapManager : public QWidget
{
  Q_OBJECT
public:
  MapManager(QWidget* parent = nullptr);
  ~MapManager();

  // viewport modifiers
  void pan(const QPoint& delta);
  // void scale(double factor, QPointF center);
  // void resizeCanvas(int new_w, int new_h);

  // IO methods
  bool saveData(const QString& filename);
  bool loadData(const QString& filename); 
  bool loadCostmap(const QString& filename);

protected:
  void paintEvent(QPaintEvent* ev) override;
  void mousePressEvent(QMouseEvent* ev) override;
  void mouseMoveEvent(QMouseEvent* ev) override;
  void mouseReleaseEvent(QMouseEvent* ev) override;
  // void wheelEvent(QWheelEvent* ev) override;

public slots:
  void setPolyCount(int poly_count);
  void setArcRoadCreateMode();
  void setStraightRoadCreateMode();
  void setDeleteRoadMode();
  void setMoveCameraMode();
  void goToZeroViewport();

private:
  Mode mode_;

  bool panning_;

  QPoint last_mouse_pos_;

  QTransform viewport_offset_;
  QTransform map_zero_offset_;

  QPixmap* costmap_img_;

  Map* map_;
  MapData* data_;
  RoadBuilder builder_;
};