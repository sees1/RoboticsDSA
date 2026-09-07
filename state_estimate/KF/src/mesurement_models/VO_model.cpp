#include "mesurement_models/VO_model.hpp"

Eigen::Matrix<double, 6, 1> VOMesurement::getMesurement()
{
  Eigen::Matrix<double, 6, 1> mesur;
  mesur << x_, y_, z_, roll_, pitch_, yaw_;
  return mesur;
}

Eigen::Matrix<double, Eigen::Dynamic, 3> VOMesurement::getMesurementJac()
{
  Eigen::Matrix<double, Eigen::Dynamic, 3> jacobian(6, 3);
  jacobian << 1, 0, 0,
              0, 1, 0,
              0, 0, 0,
              0, 0, 0,
              0, 0, 0,
              0, 0, 1;
  return jacobian;
}

// TODO: hardcode, change me
Eigen::Matrix<double, 6, 6> VOMesurement::getMesurementCov()
{
  Eigen::Matrix<double, 6, 6> cov;
  cov << 0.01, 0, 0, 0, 0, 0,
         0, 0.01, 0, 0, 0, 0,
         0, 0, 0.01, 0, 0, 0,
         0, 0, 0, 0.01, 0, 0,
         0, 0, 0, 0, 0.01, 0,
         0, 0, 0, 0, 0, 0.01;
  return cov;
}