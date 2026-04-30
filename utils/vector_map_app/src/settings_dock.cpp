#include "vector_map_app/settings_dock.hpp"

SettingsDock::SettingsDock(MapManager* copy, QWidget* parent)
: QDockWidget("Settings", parent),
  map_manager_(copy)
{
  QWidget* road_poly_count_container = new QWidget(this);
  QHBoxLayout* road_poly_layout = new QHBoxLayout(road_poly_count_container);
  l_road_poly_count_ = new QLabel("Segment count:", road_poly_count_container);
  e_road_poly_count_ = new QLineEdit(road_poly_count_container);
  e_road_poly_count_->setText(QString::number(10));
  e_road_poly_count_->setAlignment(Qt::AlignJustify);

  font_m_ = new QFontMetrics(e_road_poly_count_->font());

  // initial text size
  int road_poly_count_tw = font_m_->horizontalAdvance(e_road_poly_count_->text());
  road_poly_count_tw += 20;
  e_road_poly_count_->setFixedWidth(road_poly_count_tw);

  connect(e_road_poly_count_, &QLineEdit::textChanged, this, 
    [this](const QString& text)
    {
      QFontMetrics fm(e_road_poly_count_->font());

      int text_width = fm.horizontalAdvance(text);
      text_width += 20;
      
      e_road_poly_count_->setFixedWidth(text_width);
    }
  );

  road_poly_layout->addWidget(l_road_poly_count_);
  road_poly_layout->addWidget(e_road_poly_count_);
  road_poly_layout->setContentsMargins(0, 0, 0, 0);
  road_poly_layout->setSpacing(4);

  b_zero_ = new QPushButton("Zero");

  connect(b_zero_, &QPushButton::clicked, map_manager_, &MapManager::goToZeroViewport);
  connect(e_road_poly_count_, &QLineEdit::editingFinished, this, &SettingsDock::savePolyCount);
  connect(this, &SettingsDock::sigPassPolyCount, map_manager_, &MapManager::setPolyCount);

  QWidget* container = new QWidget(this);
  QVBoxLayout* dock_layout = new QVBoxLayout(container);
  dock_layout->addWidget(b_zero_);
  dock_layout->setSpacing(4);
  dock_layout->addWidget(road_poly_count_container);
  dock_layout->setAlignment(Qt::AlignTop);

  container->setLayout(dock_layout);
  setWidget(container);
}

SettingsDock::~SettingsDock()
{
  delete font_m_;
}

void SettingsDock::savePolyCount()
{
  poly_count_ = e_road_poly_count_->text().toInt();

  emit sigPassPolyCount(poly_count_);
}