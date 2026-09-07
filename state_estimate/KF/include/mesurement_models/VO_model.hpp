#pragma once

#include "mesurement_models/mesurement_model_base.hpp"

class VOMesurement : public Mesurement {
public:
  Eigen::Matrix<double, 6, 1> getMesurement() override;

  Eigen::Matrix<double, Eigen::Dynamic, 3> getMesurementJac() override;
  Eigen::Matrix<double, 6, 6> getMesurementCov() override;

private:
  double x_;
  double y_;
  double z_;
  double roll_;
  double pitch_;
  double yaw_;
};
