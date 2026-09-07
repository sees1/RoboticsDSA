#include "kf.hpp"


KF::KF(const MotionModelType& model_type, const Eigen::Matrix3d& system_noise)
: system_noise_(system_noise)
{
  if (model_type == MotionModelType::Ackermann)
  {
    double base = 1.0; // TODO: add via yaml
    mmodel_ = std::make_unique<AckermannModel>(base);
  }
  else
    throw std::runtime_error("KF::KF: doesn't have propriate motion model!");
}

bool KF::update(const Control& ctrl, double del_time)
{
  current_state_ += mmodel_->getMovement(ctrl, del_time);

  Eigen::Matrix3d jacobian = mmodel_->getJacobian(ctrl, del_time);
  current_cov_ = jacobian * current_cov_ * jacobian.transpose() + system_noise_;

  return true;
}

bool KF::correction(Mesurement& mesur)
{
  if (mesur.type_ == MesurementType::VO)
  {
    VOMesurement& vo_mesur = static_cast<VOMesurement&>(mesur);

    Eigen::Matrix<double, Eigen::Dynamic, 3> C = vo_mesur.getMesurementJac();

    Eigen::Matrix<double, 6, 6> vo_S = C * current_cov_ * C.transpose() + vo_mesur.getMesurementCov();
    Eigen::Matrix<double, 3, 6> kalman_gain = current_cov_ * C.transpose() * vo_S.inverse();

    Eigen::Matrix<double, 3, 1> tmp_state = current_state_;
    current_state_ = State(tmp_state + kalman_gain * (vo_mesur.getMesurement() - C * tmp_state));

    Eigen::Matrix3d I = Eigen::Matrix3d::Identity();
    current_cov_ = (I - kalman_gain * C) * current_cov_;
  }

  return true;
}