#include <QDockWidget>

#include "vector_map_app/map_manager.hpp"

class ToolsDock : public QDockWidget
{
  Q_OBJECT
public:
  ToolsDock(MapManager* copy, QWidget* parent = nullptr);

private:
  MapManager* map_manager_;

  QPushButton* b_move_camera_;
  QToolButton* b_create_road_;
  QAction* a_create_arc_road_;
  QAction* a_create_straight_road_;
  QPushButton* b_delete_road_;
};