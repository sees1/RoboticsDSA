#include "motion_models/ackermann_model.hpp"

AckermannModel::AckermannModel(double base)
: base_(base) { }

State AckermannModel::getMovement(const Control& ctrl, double del_time)
{
  State tmp;

  tmp.x_ = ctrl.long_vel_ * std::cos(ctrl.omega_) * del_time;
  tmp.y_ = ctrl.long_vel_ * std::sin(ctrl.omega_) * del_time;
  tmp.yaw_ = ctrl.omega_ * del_time;
}

Eigen::Matrix3d AckermannModel::getJacobian(const Control& ctrl, double del_time)
{
  Eigen::Matrix3d jac;
  jac << 1, 0, (-1) * ctrl.long_vel_ * std::sin(ctrl.omega_) * del_time,
         0, 1, ctrl.long_vel_ * std::cos(ctrl.omega_) * del_time,
         0, 0, 1;
  return jac;
}