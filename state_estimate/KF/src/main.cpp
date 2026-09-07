#include "kf.hpp"

int main() {
  MotionModelType type = MotionModelType::Ackermann;
  Eigen::Matrix3d system_noise;
  system_noise << 0.0, 0.0, 0.0,
                  0.0, 0.0, 0.0,
                  0.0, 0.0, 0.0; 
  KF filter(type, system_noise);
}