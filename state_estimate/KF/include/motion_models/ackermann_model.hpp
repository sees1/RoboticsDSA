#pragma once

#include "motion_models/motion_model_base.hpp"

class AckermannModel : public MotionModel {
public:
  AckermannModel(double base);

  State getMovement(const Control& ctrl, double del_time) override;
  Eigen::Matrix3d getJacobian(const Control& ctrl, double del_time) override;

private:
  double base_;
};
