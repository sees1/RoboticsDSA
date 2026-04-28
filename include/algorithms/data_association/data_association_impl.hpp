template <typename vector, typename matrix>
typename matrix::scalar computeMaha(const vector& h, const matrix& c)
{
  return (h.transpose() * c * h).eval()(0, 0);
}

template <typename T>
Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>
extractSubmatrixSymmetricalBlock(const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& m,
                                size_t block_size,
                                const std::vector<size_t>& block_indices)
{
  if (block_size < 1)
    throw std::runtime_error("extractSubmatrixSymmetricalBlock: block_size must be >= 1");
  if (m.cols() != m.rows())
    throw std::runtime_error("extractSubmatrixSymmetricalBlock: Matrix is not square");

  size_t block_count = block_indices.size();
  size_t dim = block_count * block_size;

  Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> out(dim, dim);

  for (size_t row_block = 0; row_block < block_count; ++row_block)
  {
    for (size_t col_block = 0; col_block < block_count; ++col_block)
    {
      out.block(row_block * block_size,
                col_block * block_size,
                block_size, block_size) = m.block(block_indices[row_block] * block_size,
                                                  block_indices[col_block] * block_size,
                                                  block_size, block_size);
    }
  }

  return out;
}

// TODO: add dynamic recurent computation
// now it do h^T C^(-1) h distance computation which inefficient if hypothesis H
// which is set of association is large. We can compute lower bound of distance 
// to determine wheteher the hypotesis can be rejected without inverting C.
template <AssociationMetric METRIC>
double jointPdfMetric(const DynMatrixDouble& observation_means,
                      const DynMatrixDouble& prediction_means,
                      const DynMatrixDouble& prediction_covs,
                      const AuxDataRecursiveJCBB& info,
                      const DataAssociationResult& result)
{
  size_t n = info.current_association.size();
  if (n <= 0)
    throw std::runtime_error("joint_pdf_metric: Can't process empty list of association");
  
  std::vector<size_t> indices_pred(n);
  std::vector<size_t> indices_obs(n);

  {
    size_t i = 0;
    for(auto pair : info.current_association)
    {
      indices_obs[i] = pair.first;
      indices_pred[i] = pair.second;
      i++
    }
  }

  auto cov = extractSubmatrixSymmetricalBlock(prediction_covs, 
                                              info.length_0,
                                              indices_pred);

  Eigen::VectorXd innovations(n * info.length_0); // col dim = 1. Flatten inovation
  std::for_each(info.current_association.begin(), info.current_association.end(), [size_type counter = 0,
                                                                                  &info,
                                                                                  &prediction_means,
                                                                                  &observation_means,
                                                                                  &inovations]
  (auto&& pair)
  {
    Eigen::VectorXd pred_mean = prediction_means(pair.second, Eigen::all);
    Eigen::VectorXd obs_mean = observation_means(pair.first, Eigen::all);
    Eigen::VectorXd diff = pred_mean - obs_mean;

    for(size_type i = 0; i < info.length_0; ++i)
      innovations(i + counter) = diff(i);

    counter += info.lenght_0;
  });

  auto cov_cols_count = cov.cols();
  Eigen::MatrixXd I = Eigen::MatrixXd::Identity(cov_cols_count, cov_cols_count);
  Eigen::MatrixXd cov_inv = cov.llt().solve(I).eval();

  double dist2 = computeMaha(innovations, cov_inv);

  if constexpr (METRIC == AssociationMetric::metricMaha)
    return dist2;
  else if constexpr (METRIC == AssociationMetric::metricML)
  {
    double det = cov.det();
    return std::exp(-0.5 * dist2) / (std::pow(2 * std::number::pi, info.length_0 * 0.5) * std::sqrt(det));
  }
  else
    static_assert(false, "Unsupported metric");
}

template <AssociationMetric METRIC>
void JCBBRecursive(const DynMatrixDouble& observation_means,
                    const DynMatrixDouble& prediction_means,
                    const DynMatrixDouble& prediction_covs,
                    DataAssociationResult& results,
                    const AuxDataRecursiveJCBB& info,
                    const observation_index_t curr_obs_idx)
{
  if (curr_obs_idx >= info.observation_count) // end
  {
    // it's better choise when more features are matched (so we shouldn't know is this hypotise closer than previous)
    if (info.current_association.size() > results.association.size())
    {
      results.association = info.current_association;
      results.distance = jointPdfMetric<METRIC>(observation_means,
                                                prediction_means,
                                                prediction_covs,
                                                info,
                                                results);
    }
    else if (!info.current_association.empty() && info.current_association.size() == result.association.size())
    {
      double dist2 = jointPdfMetric<METRIC>(observation_means,
                                            prediction_means,
                                            prediction_covs,
                                            info,
                                            results);
      
      if (isCloser<METRIC>(dist2, results.distance))
      {
        results.associations = info.current_association;
        results.distance = dist2;
      }
    }
  }
  else
  {
    size_t predictions_count = results.individual_compatiblitiy.rows();

    size_t potentials = std::accumulate(std::advance(results.individual_compatib_counts.begin(), curr_obs_idx + 1),
                                        results.individual_compatib_counts.end(), 0);

    for (prediction_index_t pred_idx = 0; pred_idx < predictions_count; ++pred_idx)
    {
      // TODO: add coverage of else block to respresent usless of this check
      // it was assumed that check will prevent far association (because current_association is so bad, that
      // sum of possible association in future can't make it better), but problem here that potensials is sum 
      // of all possible association(include incompatible)
      if ((info.current_association.size() + potentials) >= results.association.size()) // very weak check (don't filter at all)
      {
        if (results.individual_compatib(pred_idx, curr_obs_idx)) // fast check by idx in row
        {
          // long check (are we used this landmark in past?)
          // TODO: can we here add inverse map pred->observ to reduce complexity? 
          auto already_asigned_iter = std::find_if(info.current_association.begin(), info.current_association.end(), [pred_idx](auto&& pair)
          {
            return pair.second == pred_idx;
          });

          // pred_idx not associated yet
          if (already_asigend_iter == info.current_association.end())
          {
            // TODO: can we use common info, like results obj? (Reduce complexity)
            AusDataRecursiveJCBB new_info = info;
            new_info.current_association[curr_obs_idx] = pred_idx;
            
            results.explored_node_count++;

            JCBBRecursive<METRIC>(observation_means,
                                  prediction_means,
                                  prediction_covs,
                                  results,
                                  new_info,
                                  curr_obs_idx + 1);
          }
        }
      }
    }

    if ((info.current_association.size() + potentials) >= results.association.size())
    {
      results.explored_node_count++;
      JCBBRecursive<METRIC>(observation_means,
                            prediction_means,
                            prediction_covs,
                            results,
                            info,
                            curr_obs_idx + 1);
    }
  }
}