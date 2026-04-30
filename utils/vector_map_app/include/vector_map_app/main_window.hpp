#pragma once

#include <QMainWindow>

#include "vector_map_app/settings_dock.hpp"
#include "vector_map_app/tools_dock.hpp"
#include "vector_map_app/map_manager.hpp"

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  MainWindow(QWidget *parent = nullptr);

public:
  void onLoadCostmap();
  void onLoadData();
  void onSaveData();

private:
  MapManager* map_manager_;

  QAction* a_load_costmap_;
  QAction* a_load_data_;
  QAction* a_save_data_;

  ToolsDock* tools_dock_;
  SettingsDock* settings_dock_;
};