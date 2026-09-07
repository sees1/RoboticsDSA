#pragma once

#include "state.hpp"
#include "control.hpp"

#include <Eigen/Dense>

enum class MotionModelType {
  Ackermann = 0
};

class MotionModel {
public:
  virtual State getMovement(const Control& ctrl, double del_time) = 0;
  virtual Eigen::Matrix3d getJacobian(const Control& ctrl, double del_time) = 0;
};
