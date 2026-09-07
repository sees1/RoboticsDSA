#pragma once
#include <stdexcept>
#include <cmath>
#include <numeric>

namespace math {
  double normalCDF(double u);
  double normalQuantile(double p);
  double chi2inv(double p, size_t dim);
} // namespace math