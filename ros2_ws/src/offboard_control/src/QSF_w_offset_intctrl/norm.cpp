//
// File: norm.cpp
//
// MATLAB Coder version            : 26.1
// C/C++ source code generated on  : 26-Aug-2026 16:37:47
//

// Include Files
#include "norm.h"
#include "rt_nonfinite.h"
#include <cmath>

// Function Definitions
//
// Arguments    : const double x[3]
// Return Type  : double
//
namespace coder {
double b_norm(const double x[3])
{
  double absxk;
  double scale;
  double t;
  double y;
  boolean_T b;
  scale = 3.312168642111238E-170;
  absxk = std::abs(x[0]);
  if (absxk > 3.312168642111238E-170) {
    y = 1.0;
    scale = absxk;
  } else {
    t = absxk / 3.312168642111238E-170;
    y = t * t;
  }
  absxk = std::abs(x[1]);
  if (absxk > scale) {
    t = scale / absxk;
    y = y * t * t + 1.0;
    scale = absxk;
  } else {
    t = absxk / scale;
    y += t * t;
  }
  absxk = std::abs(x[2]);
  if (absxk > scale) {
    t = scale / absxk;
    y = y * t * t + 1.0;
    scale = absxk;
  } else {
    t = absxk / scale;
    y += t * t;
  }
  y = scale * std::sqrt(y);
  b = std::isnan(y);
  if (b) {
    int k;
    k = 0;
    int exitg1;
    do {
      exitg1 = 0;
      if (k < 3) {
        if (std::isnan(x[k])) {
          exitg1 = 1;
        } else {
          k++;
        }
      } else {
        y = rtInf;
        exitg1 = 1;
      }
    } while (exitg1 == 0);
  }
  return y;
}

} // namespace coder

//
// File trailer for norm.cpp
//
// [EOF]
//
