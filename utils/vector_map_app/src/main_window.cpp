#include "vector_map_app/main_window.hpp"

#include <ament_index_cpp/get_package_share_directory.hpp>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QAction>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent)
: QMainWindow(parent)
{
  this->setMouseTracking(true);
  this->setMinimumSize(600, 400);
  this->setWindowTitle("RoadCreator");

  QWidget* central_widget = new QWidget(this);
  central_widget->setMouseTracking(true);
  this->setCentralWidget(central_widget);

  QVBoxLayout* main_layout = new QVBoxLayout(central_widget);
  map_manager_ = new MapManager();
  main_layout->addWidget(map_manager_);

  QMenu* file_menu = menuBar()->addMenu("File");
  
  a_load_costmap_ = new QAction("Load costmap", this);
  a_load_data_    = new QAction("Load data", this);
  a_save_data_    = new QAction("Save data", this);

  std::string package_path = ament_index_cpp::get_package_share_directory("path_find");

  //check resource exist
  qDebug() << QDir(QString::fromStdString(package_path) + "/resources/icons").entryList();

  a_load_costmap_->setIcon(QIcon(QString::fromStdString(package_path) + "/resources/icons/load-data.png"));
  a_load_data_->setIcon(QIcon(QString::fromStdString(package_path) + "/resources/icons/load-data.png"));
  a_save_data_->setIcon(QIcon(QString::fromStdString(package_path) + "/resources/icons/save-as-data.png"));

  file_menu->addAction(a_load_costmap_);
  file_menu->addAction(a_load_data_);
  file_menu->addAction(a_save_data_);

  connect(a_load_costmap_, &QAction::triggered, this, &MainWindow::onLoadCostmap);
  connect(a_load_data_,    &QAction::triggered, this, &MainWindow::onLoadData);
  connect(a_save_data_,    &QAction::triggered, this, &MainWindow::onSaveData);

  tools_dock_ = new ToolsDock(map_manager_, this);
  settings_dock_ = new SettingsDock(map_manager_, this);

  addDockWidget(Qt::TopDockWidgetArea, tools_dock_);
  addDockWidget(Qt::RightDockWidgetArea, settings_dock_);
}

void MainWindow::onLoadCostmap()
{
  QString filename = QFileDialog::getOpenFileName(this, "Select file", ".", "image (*.png)");

  if (filename.isNull())
    return;
  
  if (!map_manager_->loadCostmap(filename))
    throw std::runtime_error("Error while load data from file!");
}

void MainWindow::onLoadData()
{
  QString filename = QFileDialog::getOpenFileName(this, "Select file", ".", "data file(*.dat)");

  if (filename.isNull())
    return;
  
  if (!map_manager_->loadData(filename))
    throw std::runtime_error("Error while load data from file!");
}

void MainWindow::onSaveData()
{
  QString filename = QFileDialog::getSaveFileName(this, "Select file", ".", "data file(*.dat)");

  if (filename.isNull())
    return;

  map_manager_->saveData(filename);
}