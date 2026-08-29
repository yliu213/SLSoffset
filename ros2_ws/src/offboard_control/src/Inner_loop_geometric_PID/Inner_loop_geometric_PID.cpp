//
// File: Inner_loop_geometric_PID.cpp
//
// MATLAB Coder version            : 26.1
// C/C++ source code generated on  : 24-Aug-2026 22:38:47
//

// Include Files
#include "Inner_loop_geometric_PID.h"
#include "diag.h"
#include "mldivide.h"
#include "norm.h"
#include "rt_nonfinite.h"
#include <cmath>
#include <cstring>
#include <emmintrin.h>

// Function Definitions
//
// extract elements (NED/FRD data)
//
// Arguments    : const double rpy_angles[3]
//                const double q[3]
//                const double dq[3]
//                const double Omega[3]
//                const double Rd[9]
//                const double Omegad[3]
//                const double dOmegad[3]
//                double Fl
//                const double gains[4]
//                const double physics_param[7]
//                const double L_offset[3]
//                const double eI[3]
//                double taub[3]
//                double tau[3]
//                double rate_sp_dt1[3]
//                double rate_sp_dt2[3]
//                double eI_dt[3]
// Return Type  : void
//
void Inner_loop_geometric_PID(
    const double rpy_angles[3], const double q[3], const double dq[3],
    const double Omega[3], const double Rd[9], const double Omegad[3],
    const double dOmegad[3], double Fl, const double gains[4],
    const double physics_param[7], const double L_offset[3], const double eI[3],
    double taub[3], double tau[3], double rate_sp_dt1[3], double rate_sp_dt2[3],
    double eI_dt[3])
{
  __m128d r;
  __m128d r1;
  __m128d r2;
  double I_inv[9];
  double JT[9];
  double R[9];
  double Ts_hat_tmp[9];
  double ad_tmp[9];
  double b_R[9];
  double eR_raw[9];
  double y_tmp[9];
  double F[3];
  double ad[3];
  double b_physics_param[3];
  double b_taub_tmp[3];
  double eR[3];
  double taub_tmp[3];
  double R_tmp;
  double a;
  double a_tmp;
  double b_R_tmp;
  double c_R_tmp;
  double d_R_tmp;
  double e_R_tmp;
  double f_R_tmp;
  double g_R_tmp;
  double mT;
  double y;
  int eR_raw_tmp;
  //  measurements
  R_tmp = std::cos(rpy_angles[1]);
  b_R_tmp = std::sin(rpy_angles[1]);
  c_R_tmp = std::cos(rpy_angles[2]);
  d_R_tmp = std::sin(rpy_angles[2]);
  e_R_tmp = std::sin(rpy_angles[0]);
  mT = std::cos(rpy_angles[0]);
  R[0] = R_tmp * c_R_tmp;
  R[1] = R_tmp * d_R_tmp;
  R[2] = -b_R_tmp;
  f_R_tmp = e_R_tmp * b_R_tmp;
  R[3] = f_R_tmp * c_R_tmp - mT * d_R_tmp;
  R[4] = f_R_tmp * d_R_tmp + mT * c_R_tmp;
  R[5] = e_R_tmp * R_tmp;
  g_R_tmp = mT * b_R_tmp;
  R[6] = g_R_tmp * c_R_tmp + e_R_tmp * d_R_tmp;
  R[7] = g_R_tmp * d_R_tmp - e_R_tmp * c_R_tmp;
  R[8] = mT * R_tmp;
  //  Rbi
  //  offset inertia
  //  CoM inertia
  b_physics_param[0] = 1.0 / physics_param[4];
  b_physics_param[1] = 1.0 / physics_param[5];
  b_physics_param[2] = 1.0 / physics_param[6];
  coder::diag(b_physics_param, I_inv);
  mT = physics_param[0] + physics_param[1];
  //  vector reconstruction
  std::memset(&F[0], 0, 3U * sizeof(double));
  r = _mm_loadu_pd(&R[0]);
  r1 = _mm_loadu_pd(&F[0]);
  r2 = _mm_set1_pd(0.0);
  _mm_storeu_pd(&F[0], _mm_add_pd(r1, _mm_mul_pd(r, r2)));
  F[2] += -b_R_tmp * 0.0;
  r = _mm_loadu_pd(&R[3]);
  r1 = _mm_loadu_pd(&F[0]);
  _mm_storeu_pd(&F[0], _mm_add_pd(r1, _mm_mul_pd(r, r2)));
  F[2] += R[5] * 0.0;
  r = _mm_loadu_pd(&R[6]);
  r1 = _mm_loadu_pd(&F[0]);
  _mm_storeu_pd(&F[0], _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(Fl))));
  F[2] += Fl * R[8];
  //  thrust force vector
  Ts_hat_tmp[0] = 0.0;
  Ts_hat_tmp[3] = -Omega[2];
  Ts_hat_tmp[6] = Omega[1];
  Ts_hat_tmp[1] = Omega[2];
  Ts_hat_tmp[4] = 0.0;
  Ts_hat_tmp[7] = -Omega[0];
  Ts_hat_tmp[2] = -Omega[1];
  Ts_hat_tmp[5] = Omega[0];
  Ts_hat_tmp[8] = 0.0;
  g_R_tmp = -(physics_param[1] / mT);
  a = coder::b_norm(dq);
  y = 0.0;
  std::memset(&rate_sp_dt2[0], 0, 3U * sizeof(double));
  for (int i{0}; i < 3; i++) {
    y += g_R_tmp * q[i] * F[i];
    rate_sp_dt2[i] =
        ((rate_sp_dt2[i] + q[0] * R[3 * i]) + q[1] * R[3 * i + 1]) +
        q[2] * R[3 * i + 2];
  }
  std::memset(&rate_sp_dt1[0], 0, 3U * sizeof(double));
  R_tmp = rate_sp_dt2[0];
  c_R_tmp = rate_sp_dt2[1];
  f_R_tmp = rate_sp_dt2[2];
  for (int i{0}; i < 3; i++) {
    rate_sp_dt1[i] = ((rate_sp_dt1[i] + R_tmp * Ts_hat_tmp[3 * i]) +
                      c_R_tmp * Ts_hat_tmp[3 * i + 1]) +
                     f_R_tmp * Ts_hat_tmp[3 * i + 2];
  }
  std::memset(&rate_sp_dt2[0], 0, 3U * sizeof(double));
  e_R_tmp = 0.0;
  std::memset(&eR_raw[0], 0, 9U * sizeof(double));
  f_R_tmp = rate_sp_dt1[0];
  g_R_tmp = rate_sp_dt1[1];
  R_tmp = rate_sp_dt1[2];
  for (int i{0}; i < 3; i++) {
    c_R_tmp = Ts_hat_tmp[3 * i];
    d_R_tmp = rate_sp_dt2[i] + f_R_tmp * c_R_tmp;
    r = _mm_loadu_pd(&R[0]);
    r1 = _mm_loadu_pd(&eR_raw[3 * i]);
    _mm_storeu_pd(&eR_raw[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(c_R_tmp))));
    eR_raw_tmp = 3 * i + 2;
    eR_raw[eR_raw_tmp] += -b_R_tmp * c_R_tmp;
    c_R_tmp = Ts_hat_tmp[3 * i + 1];
    d_R_tmp += g_R_tmp * c_R_tmp;
    r = _mm_loadu_pd(&R[3]);
    r1 = _mm_loadu_pd(&eR_raw[3 * i]);
    _mm_storeu_pd(&eR_raw[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(c_R_tmp))));
    eR_raw[eR_raw_tmp] += R[5] * c_R_tmp;
    c_R_tmp = Ts_hat_tmp[eR_raw_tmp];
    d_R_tmp += R_tmp * c_R_tmp;
    rate_sp_dt2[i] = d_R_tmp;
    r = _mm_loadu_pd(&R[6]);
    r1 = _mm_loadu_pd(&eR_raw[3 * i]);
    _mm_storeu_pd(&eR_raw[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(c_R_tmp))));
    eR_raw[eR_raw_tmp] += R[8] * c_R_tmp;
    e_R_tmp += d_R_tmp * L_offset[i];
  }
  a_tmp = physics_param[0] * physics_param[1] / mT;
  b_R_tmp = -(y + a_tmp * (physics_param[3] * (a * a) - e_R_tmp));
  std::memset(&b_R[0], 0, 9U * sizeof(double));
  for (int i{0}; i < 3; i++) {
    f_R_tmp = Ts_hat_tmp[3 * i];
    r = _mm_loadu_pd(&eR_raw[0]);
    r1 = _mm_loadu_pd(&b_R[3 * i]);
    _mm_storeu_pd(&b_R[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(f_R_tmp))));
    eR_raw_tmp = 3 * i + 2;
    b_R[eR_raw_tmp] += eR_raw[2] * f_R_tmp;
    f_R_tmp = Ts_hat_tmp[3 * i + 1];
    r = _mm_loadu_pd(&eR_raw[3]);
    r1 = _mm_loadu_pd(&b_R[3 * i]);
    _mm_storeu_pd(&b_R[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(f_R_tmp))));
    b_R[eR_raw_tmp] += eR_raw[5] * f_R_tmp;
    f_R_tmp = Ts_hat_tmp[eR_raw_tmp];
    r = _mm_loadu_pd(&eR_raw[6]);
    r1 = _mm_loadu_pd(&b_R[3 * i]);
    _mm_storeu_pd(&b_R[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(f_R_tmp))));
    b_R[eR_raw_tmp] += eR_raw[8] * f_R_tmp;
  }
  b_physics_param[0] = 0.0;
  b_physics_param[1] = 0.0;
  b_physics_param[2] = physics_param[2];
  std::memset(&rate_sp_dt2[0], 0, 3U * sizeof(double));
  f_R_tmp = rate_sp_dt2[0];
  g_R_tmp = rate_sp_dt2[1];
  R_tmp = rate_sp_dt2[2];
  c_R_tmp = physics_param[0];
  d_R_tmp = L_offset[0];
  e_R_tmp = L_offset[1];
  mT = L_offset[2];
  for (int i{0}; i < 3; i++) {
    a = R[i];
    y_tmp[3 * i] = a;
    y = q[i];
    f_R_tmp += a * y;
    a = R[i + 3];
    y_tmp[3 * i + 1] = a;
    g_R_tmp += a * y;
    a = R[i + 6];
    y_tmp[3 * i + 2] = a;
    R_tmp += a * y;
    rate_sp_dt1[i] =
        ((F[i] - b_R_tmp * y) / c_R_tmp +
         ((b_R[i] * d_R_tmp + b_R[i + 3] * e_R_tmp) + b_R[i + 6] * mT)) +
        b_physics_param[i];
  }
  F[0] = L_offset[1] * R_tmp - g_R_tmp * L_offset[2];
  F[1] = f_R_tmp * L_offset[2] - L_offset[0] * R_tmp;
  F[2] = L_offset[0] * g_R_tmp - f_R_tmp * L_offset[1];
  b_physics_param[0] = physics_param[4];
  b_physics_param[1] = physics_param[5];
  b_physics_param[2] = physics_param[6];
  coder::diag(b_physics_param, eR_raw);
  r = _mm_loadu_pd(&F[0]);
  _mm_storeu_pd(&JT[0], _mm_mul_pd(r, _mm_set1_pd(F[0])));
  JT[2] = F[0] * F[2];
  r = _mm_loadu_pd(&F[0]);
  _mm_storeu_pd(&JT[3], _mm_mul_pd(r, _mm_set1_pd(F[1])));
  JT[5] = F[1] * F[2];
  r = _mm_loadu_pd(&F[0]);
  _mm_storeu_pd(&JT[6], _mm_mul_pd(r, _mm_set1_pd(F[2])));
  JT[8] = F[2] * F[2];
  r = _mm_loadu_pd(&JT[0]);
  r1 = _mm_loadu_pd(&eR_raw[0]);
  r2 = _mm_set1_pd(a_tmp);
  _mm_storeu_pd(&JT[0], _mm_add_pd(r1, _mm_mul_pd(r2, r)));
  r = _mm_loadu_pd(&JT[2]);
  r1 = _mm_loadu_pd(&eR_raw[2]);
  _mm_storeu_pd(&JT[2], _mm_add_pd(r1, _mm_mul_pd(r2, r)));
  r = _mm_loadu_pd(&JT[4]);
  r1 = _mm_loadu_pd(&eR_raw[4]);
  _mm_storeu_pd(&JT[4], _mm_add_pd(r1, _mm_mul_pd(r2, r)));
  r = _mm_loadu_pd(&JT[6]);
  r1 = _mm_loadu_pd(&eR_raw[6]);
  _mm_storeu_pd(&JT[6], _mm_add_pd(r1, _mm_mul_pd(r2, r)));
  JT[8] = eR_raw[8] + a_tmp * JT[8];
  //  state-dependent inertia
  std::memset(&ad_tmp[0], 0, 9U * sizeof(double));
  std::memset(&b_R[0], 0, 9U * sizeof(double));
  for (int i{0}; i < 3; i++) {
    int b_i;
    f_R_tmp = Rd[3 * i];
    R_tmp = y_tmp[3 * i];
    r = _mm_loadu_pd(&y_tmp[0]);
    r1 = _mm_loadu_pd(&ad_tmp[3 * i]);
    _mm_storeu_pd(&ad_tmp[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(f_R_tmp))));
    r = _mm_loadu_pd(&Ts_hat_tmp[0]);
    r1 = _mm_loadu_pd(&b_R[3 * i]);
    _mm_storeu_pd(&b_R[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(R_tmp))));
    eR_raw_tmp = 3 * i + 2;
    ad_tmp[eR_raw_tmp] += y_tmp[2] * f_R_tmp;
    b_R[eR_raw_tmp] += Ts_hat_tmp[2] * R_tmp;
    b_i = 3 * i + 1;
    f_R_tmp = Rd[b_i];
    R_tmp = y_tmp[b_i];
    r = _mm_loadu_pd(&y_tmp[3]);
    r1 = _mm_loadu_pd(&ad_tmp[3 * i]);
    _mm_storeu_pd(&ad_tmp[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(f_R_tmp))));
    r = _mm_loadu_pd(&Ts_hat_tmp[3]);
    r1 = _mm_loadu_pd(&b_R[3 * i]);
    _mm_storeu_pd(&b_R[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(R_tmp))));
    ad_tmp[eR_raw_tmp] += y_tmp[5] * f_R_tmp;
    b_R[eR_raw_tmp] += Ts_hat_tmp[5] * R_tmp;
    f_R_tmp = Rd[eR_raw_tmp];
    R_tmp = y_tmp[eR_raw_tmp];
    r = _mm_loadu_pd(&y_tmp[6]);
    r1 = _mm_loadu_pd(&ad_tmp[3 * i]);
    _mm_storeu_pd(&ad_tmp[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(f_R_tmp))));
    r = _mm_loadu_pd(&Ts_hat_tmp[6]);
    r1 = _mm_loadu_pd(&b_R[3 * i]);
    _mm_storeu_pd(&b_R[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(R_tmp))));
    ad_tmp[eR_raw_tmp] += y_tmp[8] * f_R_tmp;
    b_R[eR_raw_tmp] += 0.0 * R_tmp;
  }
  std::memset(&Ts_hat_tmp[0], 0, 9U * sizeof(double));
  std::memset(&ad[0], 0, 3U * sizeof(double));
  std::memset(&b_physics_param[0], 0, 3U * sizeof(double));
  for (int i{0}; i < 3; i++) {
    f_R_tmp = Rd[3 * i];
    r = _mm_loadu_pd(&b_R[0]);
    r1 = _mm_loadu_pd(&Ts_hat_tmp[3 * i]);
    _mm_storeu_pd(&Ts_hat_tmp[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(f_R_tmp))));
    eR_raw_tmp = 3 * i + 2;
    Ts_hat_tmp[eR_raw_tmp] += b_R[2] * f_R_tmp;
    f_R_tmp = Rd[3 * i + 1];
    r = _mm_loadu_pd(&b_R[3]);
    r1 = _mm_loadu_pd(&Ts_hat_tmp[3 * i]);
    _mm_storeu_pd(&Ts_hat_tmp[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(f_R_tmp))));
    Ts_hat_tmp[eR_raw_tmp] += b_R[5] * f_R_tmp;
    f_R_tmp = Rd[eR_raw_tmp];
    r = _mm_loadu_pd(&b_R[6]);
    r1 = _mm_loadu_pd(&Ts_hat_tmp[3 * i]);
    _mm_storeu_pd(&Ts_hat_tmp[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(f_R_tmp))));
    Ts_hat_tmp[eR_raw_tmp] += b_R[8] * f_R_tmp;
    r = _mm_loadu_pd(&Ts_hat_tmp[3 * i]);
    r1 = _mm_loadu_pd(&ad[0]);
    f_R_tmp = Omegad[i];
    _mm_storeu_pd(&ad[0], _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(f_R_tmp))));
    r = _mm_loadu_pd(&ad_tmp[3 * i]);
    r1 = _mm_loadu_pd(&b_physics_param[0]);
    R_tmp = dOmegad[i];
    _mm_storeu_pd(&b_physics_param[0],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(R_tmp))));
    ad[2] += Ts_hat_tmp[eR_raw_tmp] * f_R_tmp;
    b_physics_param[2] += ad_tmp[eR_raw_tmp] * R_tmp;
  }
  //  errors
  for (int i{0}; i < 3; i++) {
    ad[i] -= b_physics_param[i];
    f_R_tmp = Rd[3 * i];
    R_tmp = Rd[3 * i + 1];
    g_R_tmp = Rd[3 * i + 2];
    for (int i1{0}; i1 < 3; i1++) {
      eR_raw_tmp = i + 3 * i1;
      eR_raw[eR_raw_tmp] = ((f_R_tmp * R[3 * i1] + R_tmp * R[3 * i1 + 1]) +
                            g_R_tmp * R[3 * i1 + 2]) -
                           ad_tmp[eR_raw_tmp];
    }
  }
  r = _mm_loadu_pd(&eR_raw[0]);
  r1 = _mm_set1_pd(0.5);
  _mm_storeu_pd(&eR_raw[0], _mm_mul_pd(r1, r));
  r = _mm_loadu_pd(&eR_raw[2]);
  _mm_storeu_pd(&eR_raw[2], _mm_mul_pd(r1, r));
  r = _mm_loadu_pd(&eR_raw[4]);
  _mm_storeu_pd(&eR_raw[4], _mm_mul_pd(r1, r));
  r = _mm_loadu_pd(&eR_raw[6]);
  _mm_storeu_pd(&eR_raw[6], _mm_mul_pd(r1, r));
  //  before vee map
  eR[0] = eR_raw[5];
  eR[1] = eR_raw[6];
  eR[2] = eR_raw[1];
  //  inner loop control laws
  g_R_tmp = L_offset[2] * L_offset[2];
  d_R_tmp = L_offset[1] * L_offset[1];
  eR_raw[0] = (d_R_tmp + g_R_tmp) * physics_param[0] + physics_param[4];
  f_R_tmp = -physics_param[0] * L_offset[0];
  R_tmp = f_R_tmp * L_offset[1];
  eR_raw[3] = R_tmp;
  c_R_tmp = f_R_tmp * L_offset[2];
  eR_raw[6] = c_R_tmp;
  eR_raw[1] = R_tmp;
  f_R_tmp = L_offset[0] * L_offset[0];
  eR_raw[4] = physics_param[5] + physics_param[0] * (f_R_tmp + g_R_tmp);
  R_tmp = -physics_param[0] * L_offset[1] * L_offset[2];
  eR_raw[7] = R_tmp;
  eR_raw[2] = c_R_tmp;
  eR_raw[5] = R_tmp;
  eR_raw[8] = physics_param[6] + physics_param[0] * (f_R_tmp + d_R_tmp);
  std::memset(&rate_sp_dt2[0], 0, 3U * sizeof(double));
  g_R_tmp = rate_sp_dt2[0];
  R_tmp = rate_sp_dt2[1];
  c_R_tmp = rate_sp_dt2[2];
  d_R_tmp = Omegad[0];
  e_R_tmp = Omegad[1];
  mT = Omegad[2];
  a = gains[0];
  y = gains[1];
  b_R_tmp = gains[2];
  for (int i{0}; i < 3; i++) {
    f_R_tmp = Omega[i];
    g_R_tmp += eR_raw[3 * i] * f_R_tmp;
    R_tmp += eR_raw[3 * i + 1] * f_R_tmp;
    c_R_tmp += eR_raw[3 * i + 2] * f_R_tmp;
    f_R_tmp -=
        (ad_tmp[i] * d_R_tmp + ad_tmp[i + 3] * e_R_tmp) + ad_tmp[i + 6] * mT;
    eI_dt[i] = f_R_tmp;
    taub_tmp[i] = (a * eR[i] + y * f_R_tmp) + b_R_tmp * eI[i];
  }
  b_taub_tmp[0] = Omega[1] * c_R_tmp - R_tmp * Omega[2];
  b_taub_tmp[1] = g_R_tmp * Omega[2] - Omega[0] * c_R_tmp;
  b_taub_tmp[2] = Omega[0] * R_tmp - g_R_tmp * Omega[1];
  b_physics_param[2] = rate_sp_dt1[2] - physics_param[2];
  std::memset(&rate_sp_dt2[0], 0, 3U * sizeof(double));
  r = _mm_loadu_pd(&y_tmp[0]);
  r1 = _mm_loadu_pd(&rate_sp_dt2[0]);
  r2 = _mm_set1_pd(physics_param[0]);
  _mm_storeu_pd(&rate_sp_dt2[0],
                _mm_add_pd(r1, _mm_mul_pd(_mm_mul_pd(r, r2),
                                          _mm_set1_pd(rate_sp_dt1[0]))));
  rate_sp_dt2[2] += physics_param[0] * y_tmp[2] * rate_sp_dt1[0];
  r = _mm_loadu_pd(&y_tmp[3]);
  r1 = _mm_loadu_pd(&rate_sp_dt2[0]);
  _mm_storeu_pd(&rate_sp_dt2[0],
                _mm_add_pd(r1, _mm_mul_pd(_mm_mul_pd(r, r2),
                                          _mm_set1_pd(rate_sp_dt1[1]))));
  rate_sp_dt2[2] += physics_param[0] * y_tmp[5] * rate_sp_dt1[1];
  r = _mm_loadu_pd(&y_tmp[6]);
  r1 = _mm_loadu_pd(&rate_sp_dt2[0]);
  _mm_storeu_pd(&rate_sp_dt2[0],
                _mm_add_pd(r1, _mm_mul_pd(_mm_mul_pd(r, r2),
                                          _mm_set1_pd(b_physics_param[2]))));
  rate_sp_dt2[2] += physics_param[0] * y_tmp[8] * b_physics_param[2];
  F[0] = L_offset[1] * rate_sp_dt2[2] - rate_sp_dt2[1] * L_offset[2];
  F[1] = rate_sp_dt2[0] * L_offset[2] - L_offset[0] * rate_sp_dt2[2];
  F[2] = L_offset[0] * rate_sp_dt2[1] - rate_sp_dt2[0] * L_offset[1];
  std::memset(&eR_raw[0], 0, 9U * sizeof(double));
  std::memset(&b_physics_param[0], 0, 3U * sizeof(double));
  f_R_tmp = ad[0];
  c_R_tmp = ad[1];
  R_tmp = ad[2];
  for (int i{0}; i < 3; i++) {
    g_R_tmp = I_inv[3 * i];
    r = _mm_loadu_pd(&JT[0]);
    r1 = _mm_loadu_pd(&eR_raw[3 * i]);
    _mm_storeu_pd(&eR_raw[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(g_R_tmp))));
    eR_raw_tmp = 3 * i + 2;
    eR_raw[eR_raw_tmp] += JT[2] * g_R_tmp;
    g_R_tmp = I_inv[3 * i + 1];
    r = _mm_loadu_pd(&JT[3]);
    r1 = _mm_loadu_pd(&eR_raw[3 * i]);
    _mm_storeu_pd(&eR_raw[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(g_R_tmp))));
    eR_raw[eR_raw_tmp] += JT[5] * g_R_tmp;
    g_R_tmp = I_inv[eR_raw_tmp];
    r = _mm_loadu_pd(&JT[6]);
    r1 = _mm_loadu_pd(&eR_raw[3 * i]);
    _mm_storeu_pd(&eR_raw[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(g_R_tmp))));
    eR_raw[eR_raw_tmp] += JT[8] * g_R_tmp;
    taub[i] = (b_taub_tmp[i] - F[i]) -
              ((JT[i] * f_R_tmp + JT[i + 3] * c_R_tmp) + JT[i + 6] * R_tmp);
    r = _mm_loadu_pd(&eR_raw[3 * i]);
    r1 = _mm_loadu_pd(&b_physics_param[0]);
    g_R_tmp = taub_tmp[i];
    _mm_storeu_pd(&b_physics_param[0],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(g_R_tmp))));
    b_physics_param[2] += eR_raw[eR_raw_tmp] * g_R_tmp;
  }
  r = _mm_loadu_pd(&taub[0]);
  r1 = _mm_loadu_pd(&b_physics_param[0]);
  _mm_storeu_pd(&taub[0], _mm_sub_pd(r, r1));
  taub[2] -= b_physics_param[2];
  //  add integral
  f_R_tmp = 0.0 * L_offset[2];
  tau[0] = taub[0] + (L_offset[1] * Fl - f_R_tmp);
  tau[1] = taub[1] + (f_R_tmp - L_offset[0] * Fl);
  tau[2] = taub[2] + (L_offset[0] * 0.0 - 0.0 * L_offset[1]);
  //  CoM torque
  //  desired angular acceleration
  //  by controller dynamics
  //  by taub relation
  //  integral derivative
  g_R_tmp = taub_tmp[0];
  f_R_tmp = taub_tmp[1];
  R_tmp = taub_tmp[2];
  c_R_tmp = gains[3];
  r = _mm_loadu_pd(&ad[0]);
  r2 = _mm_mul_pd(r, _mm_set1_pd(-1.0));
  r = _mm_loadu_pd(&I_inv[0]);
  r1 = _mm_mul_pd(r, _mm_set1_pd(g_R_tmp));
  r = _mm_loadu_pd(&I_inv[3]);
  r = _mm_mul_pd(r, _mm_set1_pd(f_R_tmp));
  r1 = _mm_add_pd(r1, r);
  r = _mm_loadu_pd(&I_inv[6]);
  r = _mm_mul_pd(r, _mm_set1_pd(R_tmp));
  r = _mm_add_pd(r1, r);
  r = _mm_sub_pd(r2, r);
  _mm_storeu_pd(&rate_sp_dt1[0], r);
  r = _mm_loadu_pd(&taub[0]);
  r1 = _mm_loadu_pd(&b_taub_tmp[0]);
  r = _mm_sub_pd(r, r1);
  r1 = _mm_loadu_pd(&F[0]);
  r = _mm_add_pd(r, r1);
  _mm_storeu_pd(&b_taub_tmp[0], r);
  r = _mm_loadu_pd(&eR[0]);
  r = _mm_mul_pd(_mm_set1_pd(c_R_tmp), r);
  r1 = _mm_loadu_pd(&eI_dt[0]);
  r = _mm_add_pd(r1, r);
  _mm_storeu_pd(&eI_dt[0], r);
  rate_sp_dt1[2] =
      -ad[2] - ((I_inv[2] * g_R_tmp + I_inv[5] * f_R_tmp) + I_inv[8] * R_tmp);
  b_taub_tmp[2] = (taub[2] - b_taub_tmp[2]) + F[2];
  eI_dt[2] += c_R_tmp * eR[2];
  coder::mldivide(JT, b_taub_tmp, rate_sp_dt2);
}

//
// File trailer for Inner_loop_geometric_PID.cpp
//
// [EOF]
//
