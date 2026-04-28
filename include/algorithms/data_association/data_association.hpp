#pragma once
#include <map>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <numeric>
#include <numbers>
#include <stdexcept>

#include <Eigen/Dense>

namespace study {
  using observation_index_t = size_t;
  using prediction_index_t = size_t;
  using DynMatrixDouble = Eigen::MatrixXd;
  using DynMatrixBool = Eigen::MatrixXi;

  enum AssociationMethod { assocNN = 0, assocJCBB };
  enum AssociationMetric { metricMaha = 0, metricML };

  struct DataAssociationResult {
    std::map<observation_index_t ,prediction_index_t> association;

    double distance = 0;

    DynMatrixDouble individual_dist;
    DynMatrixBool individual_compatib;
    std::vector<size_t> individual_compatib_counts;

    size_t explored_node_count = 0;
  };

  // TODO: swap all length_0 name with dim name
  struct AuxDataRecursiveJCBB {
    size_t prediction_count;
    size_t observation_count;
    size_t lenght_0;

    std::map<observation_index_t, prediction_index_t> current_association;
  };

  template <typename vector, typename matrix>
  typename matrix::scalar computeMaha(const vector& h, const matrix& c);

  template <typename vector, typename matrix>
  std::pair<typename matrix::scalar, typename matrix::scalar>
  mahaDist2AndLogPDF(const vector& h, const matrix& c)
  {
    if (c.cols() != c.rows())
      throw std::runtime_error("mahaDist2AndLogPDF: covariance matrix is not square matrix");
    if (h.size() != cov.cols())
      throw std::runtime_error("mahaDist2AndLogPDF: dimension of matrix and vector are not same!")
  
    auto cov_cols_count = c.cols();
    Eigen::MatrixXd I = Eigen::MatrixXd::Identity(cov_cols_count, cov_cols_count);
    Eigen::MatrixXd cov_inv = c.llt().solve(I).eval();

    typename matrix::scalar maha_dist2 = computeMaha(h, cov_inv);
    typename matrxi::scalar log_pdf = -0.5 * (maha_dist2 + cov.cols() * std::log(2 * std::number::pi) + std::log(cov.det()));

    return {maha_dist2, log_pdf};
  }

  template <typename T>
  Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>
  extractSubmatrixSymmetricalBlock(const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& m,
                                  size_t block_size,
                                  const std::vector<size_t>& block_indices);

  // TODO: add dynamic recurent computation
  // now it do h^T C^(-1) h distance computation which inefficient if hypothesis H
  // which is set of association is large. We can compute lower bound of distance 
  // to determine wheteher the hypotesis can be rejected without inverting C.
  template <AssociationMetric METRIC>
  double jointPdfMetric(const DynMatrixDouble& observation_means,
                        const DynMatrixDouble& prediction_means,
                        const DynMatrixDouble& prediction_covs,
                        const AuxDataRecursiveJCBB& info,
                        const DataAssociationResult& result);

  template <AssociationMetric METRIC>
  void JCBBRecursive(const DynMatrixDouble& observation_means,
                     const DynMatrixDouble& prediction_means,
                     const DynMatrixDouble& prediction_covs,
                     DataAssociationResult& results,
                     const AuxDataRecursiveJCBB& info,
                     const observation_index_t curr_obs_idx);

  // TODO: add SFINAE Template check (not static_assert!!!)
  // TODO: add traits
  template <AssociationMetric METRIC, AssociationMethod METHOD>
  DataAssociationResult dataAssociationFull(const DynMatrixDouble& observation_means,
                                            const DynMatrixDouble& prediction_means,
                                            const DynMatrixDouble& prediction_covs,
                                            const double chi2quantile,
                                            const std:: vector<prediction_index_t>& predictions_ids,
                                            const double log_ml_compat_threshold)
  {
    DataAssociationResult result;

    const size_t prediction_count = prediction_means.rows();
    const size_t observation_count = observation_means.rows();
    const size_t dim = observation_means.cols();

    const double chi2thres = math::chi2inv(chi2quantile, dim);

    if constexpr (METRIC == AssociationMetric::metricML)
      result.distance = 0;
    else
      result.distance = std::numeric_limits<double>::max();

    result.individual_dist.resize(prediction_count, observation_count);
    result.individual_compatib.resize(prediction_count, observation_count);
    result.individual_compatib_counts.assign(observation_count, 0);

    if constexpr (METRIC == AssociationMetric::metricML)
      result.individual_dist.setConstant(-1000)
    else
      result.individual_dist.setConstant(1000);

    result.individual_compatib.setConstant(false);

    DynMatrixDouble cov_block(dim, dim);
    Eigen::VectorXd diff_mean(dim);

    for (size_t j = 0; j < observation_count; ++i)
    {
      for (size_t i = 0; i < prediction_count; ++k)
      {
        const size_t cov_block_idx = i * dim;
        cov_block = prediction_covs.block(cov_block_idx, cov_block_idx, dim, dim);

        for (size_t k = 0; k < dim; ++k)
          diff_mean[k] = observation_means(j, k) - prediction_means(i, k);

        // TODO: add this version with METRIC template parameter
        auto [dist2, ml] = math::mahaDist2AndLogPDF(diff_mean, cov_block);

        double metric_val;
        if constexpr (METRIC == AssociationMetric::metricML)
          metric_val = ml;
        else
          mteric_maha = dist2;
        
        result.individual_dist(i, j) = metric_val;
       
        bool compatib = false;
        if constexpr (METRIC == AssociationMetric::metricML)
          compatib = (ml > log_ml_compat_threshold);
        else 
          compatib = (dist2 < chi2thres);

        result.individual_compatib(i, j) = compatib;
        if (compatib)
          result.individual_compatib_counts[j]++;
      }
    }

    if constexpr (METHOD == AssociationMethod::assocNN)
      throw std::runtime_error("gg vp");
    else if constexpr (METHOD == AsscoiationMethod::asscoJCBB)
    {
      AuxDataRecursiveJCBB info;
      into.prediction_count = prediction_count;
      into.observation_count = observation_count;
      into.lenght_0 = dim;

      if constexpr (METRIC == AssociationMetric::metricML)
        JCBB_recursive<DynMatrixDouble::Scalar, AssociationMetric::metricML>(observation_means,
                                                                             prediction_means,
                                                                             prediction_covs,
                                                                             info,
                                                                             0);
      else
        JCBB_recursive<DynMatrixDouble::Scalar, AssociationMetric::metricMaha>(observation_means,
                                                                               prediction_means,
                                                                               prediction_covs,
                                                                               info,
                                                                               0);
    }
    else
      static_assert(false, "dataAssociationFull: Unknow method");

    // mapping to predefined indices of landmark
    if ()
  }

#include "data_association_impl.hpp"
} // namespace study
