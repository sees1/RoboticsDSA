#include <Eigen/Dense>

enum class MesurementType {
  VO = 0,
  IMU = 1
};

struct Mesurement {
  MesurementType type_;

  virtual Eigen::Matrix<double, 6, 1> getMesurement() = 0;
  virtual Eigen::Matrix<double, Eigen::Dynamic, 3> getMesurementJac() = 0;
  virtual Eigen::Matrix<double, 6, 6> getMesurementCov() = 0;
};