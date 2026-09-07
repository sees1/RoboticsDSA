#pragma once

#include <Eigen/Dense>

struct State final {
  double x_;
  double y_;
  double yaw_;

  State() = default;

  State(const Eigen::Matrix<double, 3, 1>& matrix)
  {
    x_ = matrix(0, 0);
    y_ = matrix(1, 0);
    yaw_ = matrix(2, 0); 
  }

  State& operator=(const State& other)
  {
    if (this == &other)
      return *this;

    x_ = other.x_;
    y_ = other.y_;
    yaw_ = other.yaw_;

    return *this;
  }

  State& operator+=(const State& other)
  {
    x_ += other.x_;
    y_ += other.y_;
    yaw_ += other.yaw_;

    return *this;
  }

  operator Eigen::Matrix<double, 3, 1>() {
    Eigen::Matrix<double, 3, 1> res;
    res << x_, y_, yaw_;
    return res;
  }
};