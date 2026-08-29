//
// File: QSF_w_offset_intctrl.cpp
//
// MATLAB Coder version            : 26.1
// C/C++ source code generated on  : 26-Aug-2026 16:37:47
//

// Include Files
#include "QSF_w_offset_intctrl.h"
#include "norm.h"
#include "rt_nonfinite.h"
#include <cmath>
#include <cstring>
#include <emmintrin.h>

// Function Declarations
static double rt_powd_snf(double u0, double u1);

// Function Definitions
//
// Arguments    : double u0
//                double u1
// Return Type  : double
//
static double rt_powd_snf(double u0, double u1)
{
  double y;
  if (std::isnan(u0) || std::isnan(u1)) {
    y = rtNaN;
  } else {
    double d;
    y = std::abs(u0);
    d = std::abs(u1);
    if (std::isinf(u1)) {
      if (y == 1.0) {
        y = 1.0;
      } else if (y > 1.0) {
        if (u1 > 0.0) {
          y = rtInf;
        } else {
          y = 0.0;
        }
      } else if (u1 > 0.0) {
        y = 0.0;
      } else {
        y = rtInf;
      }
    } else if (d == 0.0) {
      y = 1.0;
    } else if (d == 1.0) {
      if (u1 > 0.0) {
        y = u0;
      } else {
        y = 1.0 / u0;
      }
    } else if (u1 == 2.0) {
      y = u0 * u0;
    } else if ((u1 == 0.5) && (u0 >= 0.0)) {
      y = std::sqrt(u0);
    } else if ((u0 < 0.0) && (u1 > std::floor(u1))) {
      y = rtNaN;
    } else {
      y = std::pow(u0, u1);
    }
  }
  return y;
}

//
// refs
//
// Arguments    : double mp
//                double mq
//                double l
//                double g
//                const double K1[5]
//                const double K2[5]
//                const double K3[3]
//                const double ref_traj[15]
//                const double states[12]
//                double psi
//                const double aLd[3]
//                const double Int[3]
//                double Rbd[9]
//                double *Fld_scaler
//                double *phid
//                double *thetad
// Return Type  : void
//
void QSF_w_offset_intctrl(double mp, double mq, double l, double g,
                          const double K1[5], const double K2[5],
                          const double K3[3], const double ref_traj[15],
                          const double states[12], double psi,
                          const double aLd[3], const double Int[3],
                          double Rbd[9], double *Fld_scaler, double *phid,
                          double *thetad)
{
  double b_Int[5];
  double Fld[3];
  double b1c_tmp[3];
  double b_nu3[3];
  double b[2];
  double Fld_tmp;
  double Int_tmp;
  double a;
  double a_tmp;
  double b_Int_tmp;
  double b_K2;
  double b_a_tmp;
  double c;
  double c_a_tmp;
  double ddnu3;
  double de3;
  double dnu3;
  double dr_idx_0;
  double dr_idx_1;
  double dr_idx_2;
  double e3;
  double nu3;
  //  states
  //  xp: payload position
  //  q: payload angle
  //  vp: payload velocity
  //  w: payload angluar velocity
  //  extract elements
  //  error of y3 = act -des
  e3 = states[2] - ref_traj[10];
  de3 = states[8] - ref_traj[11];
  //  integral controller
  nu3 = ((-K3[0] * Int[2] + -K3[1] * e3) + -K3[2] * de3) + ref_traj[12];
  Fld_tmp = nu3 - ref_traj[12];
  dnu3 = ((-K3[0] * e3 + -K3[1] * de3) + -K3[2] * Fld_tmp) + ref_traj[13];
  ddnu3 = ((-K3[0] * de3 + -K3[1] * Fld_tmp) + -K3[2] * (dnu3 - ref_traj[13])) +
          ref_traj[14];
  //  4th time derivative of y1&y2
  dr_idx_0 = states[5] * states[10] - states[4] * states[11];
  dr_idx_1 = states[3] * states[11] - states[5] * states[9];
  dr_idx_2 = states[4] * states[9] - states[3] * states[10];
  c = rt_powd_snf(states[5], 3.0);
  //  error of y1&y2
  //  nu1 and nu2
  //  input u
  a_tmp = nu3 - g;
  b_a_tmp = states[5] * states[5];
  c_a_tmp = mq * l;
  a = c_a_tmp * b_a_tmp / a_tmp;
  b_Int[0] = Int[0];
  b_Int[1] = states[0] - ref_traj[0];
  b_Int[2] = states[6] - ref_traj[1];
  e3 = states[3] / states[5];
  b_Int[3] = e3 * a_tmp - ref_traj[2];
  de3 = dr_idx_0 / states[5] - dr_idx_2 * states[3] / b_a_tmp;
  b_Int[4] = (e3 * dnu3 + de3 * a_tmp) - ref_traj[3];
  Fld_tmp = 0.0;
  for (int i{0}; i < 5; i++) {
    Fld_tmp += -K1[i] * b_Int[i];
  }
  b_Int[0] = Int[1];
  b_Int[1] = states[1] - ref_traj[5];
  b_Int[2] = states[7] - ref_traj[6];
  Int_tmp = states[4] / states[5];
  b_Int[3] = Int_tmp * a_tmp - ref_traj[7];
  b_Int_tmp = dr_idx_1 / states[5] - dr_idx_2 * states[4] / b_a_tmp;
  b_Int[4] = (Int_tmp * dnu3 + b_Int_tmp * a_tmp) - ref_traj[8];
  b_K2 = 0.0;
  for (int i{0}; i < 5; i++) {
    b_K2 += -K2[i] * b_Int[i];
  }
  __m128d r;
  __m128d r1;
  nu3 = 2.0 * dnu3;
  dnu3 = dr_idx_2 * dr_idx_2;
  Fld_tmp =
      (Fld_tmp + ref_traj[4]) -
      ((e3 * ddnu3 + nu3 * de3) +
       2.0 * (states[3] * dnu3 / c - dr_idx_0 * dr_idx_2 / b_a_tmp) * a_tmp);
  e3 = (b_K2 + ref_traj[9]) -
       ((Int_tmp * ddnu3 + nu3 * b_Int_tmp) +
        2.0 * (states[4] * dnu3 / c - dr_idx_1 * dr_idx_2 / b_a_tmp) * a_tmp);
  std::memset(&b[0], 0, sizeof(double) << 1);
  de3 = a * 0.0;
  b[0] += de3 * Fld_tmp;
  b[1] += a * Fld_tmp;
  b[0] += -a * e3;
  b[1] += de3 * e3;
  //  back to physical input F
  de3 = coder::b_norm(&states[9]);
  b_nu3[0] = a_tmp / states[5];
  b_nu3[1] = b[0];
  b_nu3[2] = b[1];
  e3 = mp + mq;
  Fld[0] = c_a_tmp / e3 * (de3 * de3);
  Fld[1] = 0.0;
  Fld[2] = 0.0;
  Rbd[0] = states[3] * e3;
  Rbd[3] = -states[4] * states[3] / states[5];
  Rbd[6] = (states[3] * states[3] - 1.0) / states[5];
  Rbd[1] = states[4] * e3;
  Rbd[4] = (-(states[4] * states[4]) + 1.0) / states[5];
  Rbd[7] = states[3] * states[4] / states[5];
  Rbd[2] = states[5] * e3;
  Rbd[5] = -states[4];
  Rbd[8] = states[3];
  r = _mm_loadu_pd(&b_nu3[0]);
  r1 = _mm_loadu_pd(&Fld[0]);
  _mm_storeu_pd(&b_nu3[0], _mm_add_pd(r, r1));
  e3 = b_nu3[0];
  de3 = b_nu3[1];
  Fld_tmp = b_nu3[2];
  r = _mm_loadu_pd(&Rbd[0]);
  r1 = _mm_mul_pd(r, _mm_set1_pd(e3));
  r = _mm_loadu_pd(&Rbd[3]);
  r = _mm_mul_pd(r, _mm_set1_pd(de3));
  r1 = _mm_add_pd(r1, r);
  r = _mm_loadu_pd(&Rbd[6]);
  r = _mm_mul_pd(r, _mm_set1_pd(Fld_tmp));
  r1 = _mm_add_pd(r1, r);
  r = _mm_loadu_pd(&aLd[0]);
  r = _mm_mul_pd(_mm_set1_pd(mq), r);
  r = _mm_sub_pd(r1, r);
  _mm_storeu_pd(&Fld[0], r);
  Fld[2] = ((Rbd[2] * e3 + Rbd[5] * de3) + Rbd[8] * Fld_tmp) - mq * aLd[2];
  // output FT(41)
  Fld_tmp = std::sin(psi);
  e3 = std::cos(psi);
  *thetad = std::atan((Fld[0] * e3 + Fld[1] * Fld_tmp) / Fld[2]);
  de3 = std::cos(*thetad);
  *phid = -std::atan((-Fld[0] * Fld_tmp + Fld[1] * e3) * de3 / Fld[2]);
  //  geometric decoupler
  e3 = coder::b_norm(Fld);
  *Fld_scaler = -e3;
  r = _mm_loadu_pd(&Fld[0]);
  _mm_storeu_pd(&Fld[0],
                _mm_div_pd(_mm_mul_pd(r, _mm_set1_pd(-1.0)), _mm_set1_pd(e3)));
  Fld[2] = -Fld[2] / e3;
  //  force psi -> 0
  dr_idx_2 = -std::sin(*thetad);
  b1c_tmp[0] = Fld[1] * dr_idx_2 - 0.0 * Fld[2];
  b1c_tmp[1] = de3 * Fld[2] - Fld[0] * dr_idx_2;
  b1c_tmp[2] = Fld[0] * 0.0 - de3 * Fld[1];
  e3 = coder::b_norm(b1c_tmp);
  dr_idx_0 = -(Fld[1] * b1c_tmp[2] - b1c_tmp[1] * Fld[2]) / e3;
  dr_idx_1 = -(b1c_tmp[0] * Fld[2] - Fld[0] * b1c_tmp[2]) / e3;
  dr_idx_2 = -(Fld[0] * b1c_tmp[1] - b1c_tmp[0] * Fld[1]) / e3;
  Rbd[3] = Fld[1] * dr_idx_2 - dr_idx_1 * Fld[2];
  Rbd[4] = dr_idx_0 * Fld[2] - Fld[0] * dr_idx_2;
  Rbd[5] = Fld[0] * dr_idx_1 - dr_idx_0 * Fld[1];
  Rbd[0] = dr_idx_0;
  Rbd[6] = Fld[0];
  Rbd[1] = dr_idx_1;
  Rbd[7] = Fld[1];
  Rbd[2] = dr_idx_2;
  Rbd[8] = Fld[2];
}

//
// File trailer for QSF_w_offset_intctrl.cpp
//
// [EOF]
//
