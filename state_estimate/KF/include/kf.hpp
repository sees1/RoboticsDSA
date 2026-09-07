#pragma once

#include <rclcpp/rclcpp.hpp>
#include <cmath>
#include <memory>
#include <stdexcept>

#include "mesurement_models/VO_model.hpp"
#include "motion_models/ackermann_model.hpp"

#include <Eigen/Dense>

class KF {
public:
  KF(const MotionModelType& model_type, const Eigen::Matrix3d& system_noise);

  bool update(const Control& ctrl, double del_time);
  bool correction(Mesurement& mesur);

private:
  State current_state_;
  Eigen::Matrix3d current_cov_;
  std::unique_ptr<MotionModel> mmodel_;

  Eigen::Matrix3d system_noise_;
  Eigen::Matrix3d mesurement_noise_;
};