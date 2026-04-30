#include <QDockWidget>

#include "vector_map_app/map_manager.hpp"

class SettingsDock : public QDockWidget
{
  Q_OBJECT
public:
  SettingsDock(MapManager* copy, QWidget* parent = nullptr);
  ~SettingsDock();

signals:
  void sigPassPolyCount(int poly_count);

public slots:
  void savePolyCount();

private:
  MapManager* map_manager_;

  QFontMetrics* font_m_;

  QLabel*    l_road_poly_count_;
  QLineEdit* e_road_poly_count_;

  QPushButton* b_zero_;

  int poly_count_;
};