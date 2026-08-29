//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: Flatness_mission_spfig8.cpp
//
// MATLAB Coder version            : 24.2
// C/C++ source code generated on  : 27-Aug-2026 15:40:59
//

// Include Files
#include "Flatness_mission_spfig8.h"
#include "norm.h"
#include "rt_nonfinite.h"
#include <cmath>
#include <emmintrin.h>

// Function Declarations
static double rt_powd_snf(double u0, double u1);

static void unitvec_dt(const double v[3], const double dv[3],
                       const double ddv[3], const double dddv[3],
                       const double ddddv[3], double du[3], double ddu[3],
                       double d3u[3], double d4u[3]);

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
// Arguments    : const double v[3]
//                const double dv[3]
//                const double ddv[3]
//                const double dddv[3]
//                const double ddddv[3]
//                double du[3]
//                double ddu[3]
//                double d3u[3]
//                double d4u[3]
// Return Type  : void
//
static void unitvec_dt(const double v[3], const double dv[3],
                       const double ddv[3], const double dddv[3],
                       const double ddddv[3], double du[3], double ddu[3],
                       double d3u[3], double d4u[3])
{
  static const signed char b_I[9]{1, 0, 0, 0, 1, 0, 0, 0, 1};
  __m128d r;
  __m128d r1;
  __m128d r2;
  __m128d r3;
  __m128d r4;
  double P[9];
  double c_ddu[9];
  double c_u[9];
  double d3u_tmp[9];
  double dP[9];
  double b_P[3];
  double u[3];
  double a;
  double a_tmp;
  double b;
  double b_a;
  double b_a_tmp;
  double b_b;
  double b_ddu;
  double b_du;
  double b_tmp;
  double b_u;
  double c_a;
  double c_a_tmp;
  double d;
  double d1;
  double d2;
  double d3;
  double d4;
  double d5;
  double d6;
  double d7;
  double d8;
  double d9;
  double d_a;
  double d_a_tmp;
  double dds;
  double ddu_tmp;
  double e_a;
  double f_a;
  double g_a;
  double s;
  int P_tmp;
  s = coder::b_norm(v);
  r = _mm_set1_pd(s);
  _mm_storeu_pd(&u[0], _mm_div_pd(_mm_loadu_pd(&v[0]), r));
  u[2] = v[2] / s;
  b_u = 0.0;
  for (int i{0}; i < 3; i++) {
    P[3 * i] = static_cast<double>(b_I[3 * i]) - u[0] * u[i];
    P_tmp = 3 * i + 1;
    P[P_tmp] = static_cast<double>(b_I[P_tmp]) - u[1] * u[i];
    P_tmp = 3 * i + 2;
    P[P_tmp] = static_cast<double>(b_I[P_tmp]) - u[2] * u[i];
    b_u += u[i] * dv[i];
  }
  a_tmp = 1.0 / s;
  d = dv[0];
  d1 = dv[1];
  d2 = dv[2];
  r1 = _mm_loadu_pd(&P[0]);
  r1 = _mm_mul_pd(r1, _mm_set1_pd(d));
  r2 = _mm_loadu_pd(&P[3]);
  r2 = _mm_mul_pd(r2, _mm_set1_pd(d1));
  r1 = _mm_add_pd(r1, r2);
  r2 = _mm_loadu_pd(&P[6]);
  r2 = _mm_mul_pd(r2, _mm_set1_pd(d2));
  r1 = _mm_add_pd(r1, r2);
  r1 = _mm_mul_pd(_mm_set1_pd(a_tmp), r1);
  _mm_storeu_pd(&du[0], r1);
  du[2] = a_tmp * ((P[2] * d + P[5] * d1) + P[8] * d2);
  ddu_tmp = coder::b_norm(du);
  b_a_tmp = b_u / s;
  c_a_tmp = 2.0 * b_a_tmp;
  b_tmp = ddu_tmp * ddu_tmp;
  d = ddv[0];
  d1 = ddv[1];
  d2 = ddv[2];
  for (int i{0}; i < 3; i++) {
    dP[3 * i] = du[0] * u[i];
    c_u[3 * i] = u[0] * du[i];
    P_tmp = 3 * i + 1;
    dP[P_tmp] = du[1] * u[i];
    c_u[P_tmp] = u[1] * du[i];
    P_tmp = 3 * i + 2;
    dP[P_tmp] = du[2] * u[i];
    c_u[P_tmp] = u[2] * du[i];
    ddu[i] = (a_tmp * ((P[i] * d + P[i + 3] * d1) + P[i + 6] * d2) -
              c_a_tmp * du[i]) -
             u[i] * b_tmp;
  }
  r1 = _mm_loadu_pd(&dP[0]);
  r2 = _mm_loadu_pd(&c_u[0]);
  r3 = _mm_set1_pd(-1.0);
  _mm_storeu_pd(&dP[0], _mm_mul_pd(_mm_add_pd(r1, r2), r3));
  r1 = _mm_loadu_pd(&dP[2]);
  r2 = _mm_loadu_pd(&c_u[2]);
  _mm_storeu_pd(&dP[2], _mm_mul_pd(_mm_add_pd(r1, r2), r3));
  r1 = _mm_loadu_pd(&dP[4]);
  r2 = _mm_loadu_pd(&c_u[4]);
  _mm_storeu_pd(&dP[4], _mm_mul_pd(_mm_add_pd(r1, r2), r3));
  r1 = _mm_loadu_pd(&dP[6]);
  r2 = _mm_loadu_pd(&c_u[6]);
  _mm_storeu_pd(&dP[6], _mm_mul_pd(_mm_add_pd(r1, r2), r3));
  dP[8] = -(dP[8] + c_u[8]);
  dds = ((du[0] * dv[0] + du[1] * dv[1]) + du[2] * dv[2]) +
        ((u[0] * ddv[0] + u[1] * ddv[1]) + u[2] * ddv[2]);
  //  Helper scalars
  d_a_tmp = s * s;
  ddu_tmp = b_u * b_u;
  a = dds / s - ddu_tmp / d_a_tmp;
  //  db = a;
  //  Third derivative
  r1 = _mm_loadu_pd(&P[0]);
  r2 = _mm_loadu_pd(&dP[0]);
  r4 = _mm_set1_pd(b_a_tmp);
  _mm_storeu_pd(&d3u_tmp[0], _mm_sub_pd(r2, _mm_mul_pd(r4, r1)));
  r1 = _mm_loadu_pd(&P[2]);
  r2 = _mm_loadu_pd(&dP[2]);
  _mm_storeu_pd(&d3u_tmp[2], _mm_sub_pd(r2, _mm_mul_pd(r4, r1)));
  r1 = _mm_loadu_pd(&P[4]);
  r2 = _mm_loadu_pd(&dP[4]);
  _mm_storeu_pd(&d3u_tmp[4], _mm_sub_pd(r2, _mm_mul_pd(r4, r1)));
  r1 = _mm_loadu_pd(&P[6]);
  r2 = _mm_loadu_pd(&dP[6]);
  _mm_storeu_pd(&d3u_tmp[6], _mm_sub_pd(r2, _mm_mul_pd(r4, r1)));
  d3u_tmp[8] = dP[8] - b_a_tmp * P[8];
  b_du = (du[0] * ddu[0] + du[1] * ddu[1]) + du[2] * ddu[2];
  b_a = 2.0 * a;
  c_a = 2.0 * b_du;
  //  fourth derivative
  d_a = 2.0 / s;
  b = b_u / d_a_tmp;
  e_a = dds / d_a_tmp;
  b_b = ddu_tmp / rt_powd_snf(s, 3.0);
  ddu_tmp = 0.0;
  b_a_tmp = 0.0;
  d = 0.0;
  f_a = 4.0 * a;
  g_a = 4.0 * b_du;
  b_ddu = 0.0;
  b_du = 0.0;
  d1 = ddddv[0];
  d2 = ddddv[1];
  d3 = ddddv[2];
  for (int i{0}; i < 3; i++) {
    double d10;
    d4 = P[i + 3];
    d5 = P[i + 6];
    d6 = P[i];
    d7 = du[i];
    d8 = ddu[i];
    d9 = u[i];
    d10 = ((((a_tmp * ((d6 * dddv[0] + d4 * dddv[1]) + d5 * dddv[2]) +
              ((a_tmp * d3u_tmp[i] * ddv[0] + a_tmp * d3u_tmp[i + 3] * ddv[1]) +
               a_tmp * d3u_tmp[i + 6] * ddv[2])) -
             b_a * d7) -
            c_a_tmp * d8) -
           c_a * d9) -
          d7 * b_tmp;
    d3u[i] = d10;
    ddu_tmp += d9 * dddv[i];
    b_a_tmp += d8 * dv[i];
    d += 2.0 * d7 * ddv[i];
    b_ddu += d8 * d8;
    b_du += d7 * d10;
    c_ddu[3 * i] = ddu[0] * d9 + 2.0 * (du[0] * du[i]);
    c_u[3 * i] = u[0] * ddu[i];
    P_tmp = 3 * i + 1;
    c_ddu[P_tmp] = ddu[1] * u[i] + 2.0 * (du[1] * du[i]);
    c_u[P_tmp] = u[1] * ddu[i];
    P_tmp = 3 * i + 2;
    c_ddu[P_tmp] = ddu[2] * u[i] + 2.0 * (du[2] * du[i]);
    c_u[P_tmp] = u[2] * ddu[i];
    b_P[i] = (d6 * d1 + d4 * d2) + d5 * d3;
  }
  __m128d r10;
  __m128d r5;
  __m128d r6;
  __m128d r7;
  __m128d r8;
  __m128d r9;
  a = 2.0 *
      ((((ddu_tmp + b_a_tmp) + d) / s - dds * b_u / d_a_tmp) - c_a_tmp * a);
  ddu_tmp = b_ddu + b_du;
  r1 = _mm_loadu_pd(&dP[0]);
  r2 = _mm_loadu_pd(&c_ddu[0]);
  r4 = _mm_loadu_pd(&c_u[0]);
  r5 = _mm_loadu_pd(&P[0]);
  r6 = _mm_set1_pd(-2.0);
  r7 = _mm_set1_pd(b);
  r8 = _mm_set1_pd(e_a);
  r9 = _mm_set1_pd(2.0);
  r10 = _mm_set1_pd(b_b);
  _mm_storeu_pd(
      &dP[0],
      _mm_add_pd(
          _mm_sub_pd(
              _mm_add_pd(_mm_mul_pd(_mm_mul_pd(r6, r1), r7),
                         _mm_div_pd(_mm_mul_pd(_mm_add_pd(r2, r4), r3), r)),
              _mm_mul_pd(r8, r5)),
          _mm_mul_pd(_mm_mul_pd(r5, r9), r10)));
  r1 = _mm_loadu_pd(&dP[2]);
  r2 = _mm_loadu_pd(&c_ddu[2]);
  r4 = _mm_loadu_pd(&c_u[2]);
  r5 = _mm_loadu_pd(&P[2]);
  _mm_storeu_pd(
      &dP[2],
      _mm_add_pd(
          _mm_sub_pd(
              _mm_add_pd(_mm_mul_pd(_mm_mul_pd(r6, r1), r7),
                         _mm_div_pd(_mm_mul_pd(_mm_add_pd(r2, r4), r3), r)),
              _mm_mul_pd(r8, r5)),
          _mm_mul_pd(_mm_mul_pd(r5, r9), r10)));
  r1 = _mm_loadu_pd(&dP[4]);
  r2 = _mm_loadu_pd(&c_ddu[4]);
  r4 = _mm_loadu_pd(&c_u[4]);
  r5 = _mm_loadu_pd(&P[4]);
  _mm_storeu_pd(
      &dP[4],
      _mm_add_pd(
          _mm_sub_pd(
              _mm_add_pd(_mm_mul_pd(_mm_mul_pd(r6, r1), r7),
                         _mm_div_pd(_mm_mul_pd(_mm_add_pd(r2, r4), r3), r)),
              _mm_mul_pd(r8, r5)),
          _mm_mul_pd(_mm_mul_pd(r5, r9), r10)));
  r1 = _mm_loadu_pd(&dP[6]);
  r2 = _mm_loadu_pd(&c_ddu[6]);
  r4 = _mm_loadu_pd(&c_u[6]);
  r5 = _mm_loadu_pd(&P[6]);
  _mm_storeu_pd(
      &dP[6],
      _mm_add_pd(
          _mm_sub_pd(
              _mm_add_pd(_mm_mul_pd(_mm_mul_pd(r6, r1), r7),
                         _mm_div_pd(_mm_mul_pd(_mm_add_pd(r2, r4), r3), r)),
              _mm_mul_pd(r8, r5)),
          _mm_mul_pd(_mm_mul_pd(r5, r9), r10)));
  dP[8] = ((-2.0 * dP[8] * b + -(c_ddu[8] + c_u[8]) / s) - e_a * P[8]) +
          P[8] * 2.0 * b_b;
  d = dddv[0];
  d1 = dddv[1];
  d2 = dddv[2];
  d3 = ddv[0];
  d4 = ddv[1];
  d5 = ddv[2];
  r = _mm_loadu_pd(&b_P[0]);
  r = _mm_mul_pd(_mm_set1_pd(a_tmp), r);
  r1 = _mm_loadu_pd(&d3u_tmp[0]);
  r1 = _mm_mul_pd(_mm_set1_pd(d_a), r1);
  r1 = _mm_mul_pd(r1, _mm_set1_pd(d));
  r2 = _mm_loadu_pd(&d3u_tmp[3]);
  r2 = _mm_mul_pd(_mm_set1_pd(d_a), r2);
  r2 = _mm_mul_pd(r2, _mm_set1_pd(d1));
  r1 = _mm_add_pd(r1, r2);
  r2 = _mm_loadu_pd(&d3u_tmp[6]);
  r2 = _mm_mul_pd(_mm_set1_pd(d_a), r2);
  r2 = _mm_mul_pd(r2, _mm_set1_pd(d2));
  r1 = _mm_add_pd(r1, r2);
  r = _mm_add_pd(r, r1);
  r1 = _mm_loadu_pd(&dP[0]);
  r1 = _mm_mul_pd(r1, _mm_set1_pd(d3));
  r2 = _mm_loadu_pd(&dP[3]);
  r2 = _mm_mul_pd(r2, _mm_set1_pd(d4));
  r1 = _mm_add_pd(r1, r2);
  r2 = _mm_loadu_pd(&dP[6]);
  r2 = _mm_mul_pd(r2, _mm_set1_pd(d5));
  r1 = _mm_add_pd(r1, r2);
  _mm_storeu_pd(&b_P[0], r1);
  r = _mm_add_pd(r, r1);
  r1 = _mm_loadu_pd(&du[0]);
  r2 = _mm_mul_pd(_mm_set1_pd(a), r1);
  r = _mm_sub_pd(r, r2);
  r2 = _mm_loadu_pd(&ddu[0]);
  r3 = _mm_mul_pd(_mm_set1_pd(f_a), r2);
  r = _mm_sub_pd(r, r3);
  r3 = _mm_loadu_pd(&d3u[0]);
  r3 = _mm_mul_pd(_mm_set1_pd(c_a_tmp), r3);
  r = _mm_sub_pd(r, r3);
  r2 = _mm_mul_pd(r2, _mm_set1_pd(b_tmp));
  r = _mm_sub_pd(r, r2);
  r1 = _mm_mul_pd(_mm_set1_pd(g_a), r1);
  r = _mm_sub_pd(r, r1);
  r1 = _mm_loadu_pd(&u[0]);
  r1 = _mm_mul_pd(_mm_set1_pd(2.0), r1);
  r1 = _mm_mul_pd(r1, _mm_set1_pd(ddu_tmp));
  r = _mm_sub_pd(r, r1);
  _mm_storeu_pd(&d4u[0], r);
  d6 = a_tmp * b_P[2] +
       ((d_a * d3u_tmp[2] * d + d_a * d3u_tmp[5] * d1) + d_a * d3u_tmp[8] * d2);
  d7 = (dP[2] * d3 + dP[5] * d4) + dP[8] * d5;
  d8 = du[2];
  d9 = ddu[2];
  d6 = ((((((d6 + d7) - a * d8) - f_a * d9) - c_a_tmp * d3u[2]) - d9 * b_tmp) -
        g_a * d8) -
       2.0 * u[2] * ddu_tmp;
  d4u[2] = d6;
}

//
// Arguments    : double t
//                double mp
//                double mq
//                double g
//                double l
//                const double L_offset[3]
//                double phi
//                double theta
//                double psi
//                const double Omega[3]
//                double A
//                double B
//                double w
//                double Od[3]
//                double dOd[3]
//                double aLd[3]
// Return Type  : void
//
void Flatness_mission_spfig8(double t, double mp, double mq, double g, double l,
                             const double L_offset[3], double phi, double theta,
                             double psi, const double Omega[3], double A,
                             double B, double w, double Od[3], double dOd[3],
                             double aLd[3])
{
  __m128d r;
  __m128d r1;
  __m128d r2;
  double b_dRb[9];
  double d2Rb[9];
  double dRb[9];
  double y_tmp[9];
  double T[3];
  double b3c[3];
  double b_T[3];
  double b_d2T[3];
  double b_dT[3];
  double b_mp[3];
  double c[3];
  double c_mp[3];
  double d2T[3];
  double d3qd[3];
  double d4qd[3];
  double dT[3];
  double dc[3];
  double ddc[3];
  double b1c_idx_1;
  double b1c_idx_2;
  double d;
  double d1;
  double d2;
  double d2xpd;
  double d2xpd_tmp_tmp;
  double d2ypd;
  double d3;
  double d3xpd;
  double d3ypd;
  double d4;
  double d4xpd;
  double d4ypd;
  double d5;
  double d5xpd;
  double d5ypd;
  double d6xpd;
  double d6ypd;
  d = std::cos(theta);
  d1 = std::sin(theta);
  d2 = std::cos(psi);
  d3 = std::sin(psi);
  d4 = std::sin(phi);
  d5 = std::cos(phi);
  //  desired load position and derivatives
  //  initialize derivatives
  d2xpd = 0.0;
  d2ypd = 0.0;
  d3xpd = 0.0;
  d3ypd = 0.0;
  d4xpd = 0.0;
  d4ypd = 0.0;
  d5xpd = 0.0;
  d5ypd = 0.0;
  d6xpd = 0.0;
  d6ypd = 0.0;
  if ((!(t < 15.0)) && (!(t < 30.0)) && (!(t < 45.0)) && (!(t < 60.0)) &&
      (t < 105.0)) {
    double d2ypd_tmp;
    //  Figure-8 starts at t = 60 s
    //  position
    //  velocity
    //  acceleration
    d2xpd_tmp_tmp = w * (t - 60.0);
    b1c_idx_2 = std::sin(d2xpd_tmp_tmp);
    d6ypd = w * w;
    d2xpd = -A * d6ypd * b1c_idx_2;
    d6xpd = 2.0 * w * (t - 60.0);
    d2ypd_tmp = std::sin(d6xpd);
    d2ypd = -2.0 * B * d6ypd * d2ypd_tmp;
    //  jerk
    d5xpd = std::cos(d2xpd_tmp_tmp);
    d6ypd = rt_powd_snf(w, 3.0);
    d3xpd = -A * d6ypd * d5xpd;
    b1c_idx_1 = std::cos(d6xpd);
    d3ypd = -4.0 * B * d6ypd * b1c_idx_1;
    //  snap
    d6xpd = rt_powd_snf(w, 4.0);
    d4xpd = A * d6xpd * b1c_idx_2;
    d4ypd = 8.0 * B * d6xpd * d2ypd_tmp;
    //  5th derivative
    d2xpd_tmp_tmp = rt_powd_snf(w, 5.0);
    d5xpd *= A * d2xpd_tmp_tmp;
    d5ypd = 16.0 * B * d2xpd_tmp_tmp * b1c_idx_1;
    //  6th derivative
    d2xpd_tmp_tmp = rt_powd_snf(w, 6.0);
    d6xpd = -A * d2xpd_tmp_tmp * b1c_idx_2;
    d6ypd = -32.0 * B * d2xpd_tmp_tmp * d2ypd_tmp;
  }
  //  differential-flatness generator
  //  1) desired tension force vector
  T[0] = mp * d2xpd;
  T[1] = mp * d2ypd;
  T[2] = mp * 0.0 - mp * g;
  dT[0] = mp * d3xpd;
  dT[1] = mp * d3ypd;
  dT[2] = mp * 0.0;
  d2T[0] = mp * d4xpd;
  d2T[1] = mp * d4ypd;
  d2T[2] = mp * 0.0;
  //  2) get q
  //  q := p3
  r = _mm_loadu_pd(&T[0]);
  r1 = _mm_set1_pd(-1.0);
  _mm_storeu_pd(&b_T[0], _mm_mul_pd(r, r1));
  r = _mm_loadu_pd(&dT[0]);
  _mm_storeu_pd(&b_dT[0], _mm_mul_pd(r, r1));
  r = _mm_loadu_pd(&d2T[0]);
  _mm_storeu_pd(&b_d2T[0], _mm_mul_pd(r, r1));
  b_T[2] = -T[2];
  b_dT[2] = -dT[2];
  b_d2T[2] = -d2T[2];
  b_mp[0] = -(mp * d5xpd);
  b_mp[1] = -(mp * d5ypd);
  b_mp[2] = -(mp * 0.0);
  c_mp[0] = -(mp * d6xpd);
  c_mp[1] = -(mp * d6ypd);
  c_mp[2] = -(mp * 0.0);
  unitvec_dt(b_T, b_dT, b_d2T, b_mp, c_mp, dc, ddc, d3qd, d4qd);
  //  since q = -T/norm(T) := p3
  //  3) find suspension point acc. (p = pl - q*l)
  r = _mm_loadu_pd(&ddc[0]);
  r2 = _mm_set1_pd(l);
  _mm_storeu_pd(&ddc[0], _mm_mul_pd(r, r2));
  r = _mm_loadu_pd(&d3qd[0]);
  _mm_storeu_pd(&d3qd[0], _mm_mul_pd(r, r2));
  r = _mm_loadu_pd(&d4qd[0]);
  _mm_storeu_pd(&d4qd[0], _mm_mul_pd(r, r2));
  ddc[2] *= l;
  d3qd[2] *= l;
  d4qd[2] *= l;
  //  4) find Rbi under low angular acc.
  c[0] = Omega[1] * L_offset[2] - L_offset[1] * Omega[2];
  c[1] = L_offset[0] * Omega[2] - Omega[0] * L_offset[2];
  c[2] = Omega[0] * L_offset[1] - L_offset[0] * Omega[1];
  dRb[0] = d * d2;
  dRb[1] = d * d3;
  dRb[2] = -d1;
  d6ypd = d4 * d1;
  dRb[3] = d6ypd * d2 - d5 * d3;
  dRb[4] = d6ypd * d3 + d5 * d2;
  dRb[5] = d4 * d;
  d6ypd = d5 * d1;
  dRb[6] = d6ypd * d2 + d4 * d3;
  dRb[7] = d6ypd * d3 - d4 * d2;
  dRb[8] = d5 * d;
  b_dT[0] = d2xpd - ddc[0];
  b_dT[1] = d2ypd - ddc[1];
  b_dT[2] = 0.0 - ddc[2];
  b_T[0] = 0.0;
  b_T[1] = 0.0;
  b_T[2] = mq * g;
  d = Omega[1] * c[2] - c[1] * Omega[2];
  d1 = c[0] * Omega[2] - Omega[0] * c[2];
  d4 = Omega[0] * c[1] - c[0] * Omega[1];
  r = _mm_loadu_pd(&dRb[0]);
  r = _mm_mul_pd(r, _mm_set1_pd(d));
  r2 = _mm_loadu_pd(&dRb[3]);
  r2 = _mm_mul_pd(r2, _mm_set1_pd(d1));
  r = _mm_add_pd(r, r2);
  r2 = _mm_loadu_pd(&dRb[6]);
  r2 = _mm_mul_pd(r2, _mm_set1_pd(d4));
  r = _mm_add_pd(r, r2);
  r2 = _mm_loadu_pd(&b_dT[0]);
  r = _mm_sub_pd(r2, r);
  r = _mm_mul_pd(_mm_set1_pd(mq), r);
  r2 = _mm_loadu_pd(&T[0]);
  r = _mm_add_pd(r2, r);
  r2 = _mm_loadu_pd(&b_T[0]);
  r = _mm_sub_pd(r, r2);
  _mm_storeu_pd(&b_dT[0], r);
  b_dT[2] =
      (T[2] + mq * (b_dT[2] - ((dRb[2] * d + dRb[5] * d1) + dRb[8] * d4))) -
      b_T[2];
  d6ypd = std::atan((b_dT[0] * d2 + b_dT[1] * d3) / b_dT[2]);
  d = coder::b_norm(b_dT);
  d6xpd = std::cos(d6ypd);
  d6ypd = -std::sin(d6ypd);
  //  directly by reference
  r = _mm_loadu_pd(&b_dT[0]);
  r = _mm_mul_pd(r, r1);
  _mm_storeu_pd(&b3c[0], _mm_div_pd(r, _mm_set1_pd(d)));
  _mm_storeu_pd(&b_mp[0], r);
  b3c[2] = -b_dT[2] / d;
  b_mp[2] = -b_dT[2];
  c[0] = b3c[1] * d6ypd - 0.0 * b3c[2];
  c[1] = d6xpd * b3c[2] - b3c[0] * d6ypd;
  c[2] = b3c[0] * 0.0 - d6xpd * b3c[1];
  d5ypd = coder::b_norm(c);
  d5xpd = -(b3c[1] * c[2] - c[1] * b3c[2]) / d5ypd;
  b1c_idx_1 = -(c[0] * b3c[2] - b3c[0] * c[2]) / d5ypd;
  b1c_idx_2 = -(b3c[0] * c[1] - c[0] * b3c[1]) / d5ypd;
  b_dT[0] = -(dT[0] + mq * (d3xpd - d3qd[0]));
  b_dT[1] = -(dT[1] + mq * (d3ypd - d3qd[1]));
  b_dT[2] = -(dT[2] + mq * (0.0 - d3qd[2]));
  b_d2T[0] = -(d2T[0] + mq * (d4xpd - d4qd[0]));
  b_d2T[1] = -(d2T[1] + mq * (d4ypd - d4qd[1]));
  b_d2T[2] = -(d2T[2] + mq * (0.0 - d4qd[2]));
  d2T[0] = 0.0;
  b_T[0] = 0.0;
  d2T[1] = 0.0;
  b_T[1] = 0.0;
  d2T[2] = 0.0;
  b_T[2] = 0.0;
  unitvec_dt(b_mp, b_dT, b_d2T, d2T, b_T, c_mp, dT, T, dc);
  //  since b3 = -F/norm(F)
  //  1) assume derivatives of b1d = 0
  //  get 1-4th derivative of b1
  dc[0] = c_mp[1] * d6ypd - 0.0 * c_mp[2];
  dc[1] = d6xpd * c_mp[2] - c_mp[0] * d6ypd;
  dc[2] = c_mp[0] * 0.0 - d6xpd * c_mp[1];
  ddc[0] = dT[1] * d6ypd - 0.0 * dT[2];
  ddc[1] = d6xpd * dT[2] - dT[0] * d6ypd;
  ddc[2] = dT[0] * 0.0 - d6xpd * dT[1];
  d6xpd = ((c[0] * dc[0] + c[1] * dc[1]) + c[2] * dc[2]) / d5ypd;
  d6ypd = d6xpd / d5ypd;
  b_dT[0] =
      (-(c_mp[1] * c[2] - c[1] * c_mp[2]) - (b3c[1] * dc[2] - dc[1] * b3c[2])) /
          d5ypd -
      d6ypd * d5xpd;
  b_dT[1] =
      (-(c[0] * c_mp[2] - c_mp[0] * c[2]) - (dc[0] * b3c[2] - b3c[0] * dc[2])) /
          d5ypd -
      d6ypd * b1c_idx_1;
  b_dT[2] =
      (-(c_mp[0] * c[1] - c[0] * c_mp[1]) - (b3c[0] * dc[1] - dc[0] * b3c[1])) /
          d5ypd -
      d6ypd * b1c_idx_2;
  d2xpd_tmp_tmp = 2.0 * d6ypd;
  d6ypd = ((((dc[0] * dc[0] + dc[1] * dc[1]) + dc[2] * dc[2]) +
            ((c[0] * ddc[0] + c[1] * ddc[1]) + c[2] * ddc[2])) -
           d6xpd * d6xpd) /
          d5ypd / d5ypd;
  T[0] = (((-(dT[1] * c[2] - c[1] * dT[2]) -
            2.0 * (c_mp[1] * dc[2] - dc[1] * c_mp[2])) -
           (b3c[1] * ddc[2] - ddc[1] * b3c[2])) /
              d5ypd -
          d2xpd_tmp_tmp * b_dT[0]) -
         d6ypd * d5xpd;
  T[1] = (((-(c[0] * dT[2] - dT[0] * c[2]) -
            2.0 * (dc[0] * c_mp[2] - c_mp[0] * dc[2])) -
           (ddc[0] * b3c[2] - b3c[0] * ddc[2])) /
              d5ypd -
          d2xpd_tmp_tmp * b_dT[1]) -
         d6ypd * b1c_idx_1;
  T[2] = (((-(dT[0] * c[1] - c[0] * dT[1]) -
            2.0 * (c_mp[0] * dc[1] - dc[0] * c_mp[1])) -
           (b3c[0] * ddc[1] - ddc[0] * b3c[1])) /
              d5ypd -
          d2xpd_tmp_tmp * b_dT[2]) -
         d6ypd * b1c_idx_2;
  //  desired Omega
  y_tmp[1] = b3c[1] * b1c_idx_2 - b1c_idx_1 * b3c[2];
  y_tmp[4] = d5xpd * b3c[2] - b3c[0] * b1c_idx_2;
  y_tmp[7] = b3c[0] * b1c_idx_1 - d5xpd * b3c[1];
  dRb[0] = b_dT[0];
  dRb[3] = (c_mp[1] * b1c_idx_2 - b1c_idx_1 * c_mp[2]) +
           (b3c[1] * b_dT[2] - b_dT[1] * b3c[2]);
  dRb[6] = c_mp[0];
  y_tmp[0] = d5xpd;
  y_tmp[2] = b3c[0];
  dRb[1] = b_dT[1];
  dRb[4] = (d5xpd * c_mp[2] - c_mp[0] * b1c_idx_2) +
           (b_dT[0] * b3c[2] - b3c[0] * b_dT[2]);
  dRb[7] = c_mp[1];
  y_tmp[3] = b1c_idx_1;
  y_tmp[5] = b3c[1];
  dRb[2] = b_dT[2];
  dRb[5] = (c_mp[0] * b1c_idx_1 - d5xpd * c_mp[1]) +
           (b3c[0] * b_dT[1] - b_dT[0] * b3c[1]);
  dRb[8] = c_mp[2];
  y_tmp[6] = b1c_idx_2;
  y_tmp[8] = b3c[2];
  for (int i{0}; i < 3; i++) {
    d = y_tmp[i];
    d1 = y_tmp[i + 3];
    d2 = y_tmp[i + 6];
    for (int i1{0}; i1 < 3; i1++) {
      d2Rb[i + 3 * i1] =
          (d * dRb[3 * i1] + d1 * dRb[3 * i1 + 1]) + d2 * dRb[3 * i1 + 2];
    }
  }
  Od[0] = d2Rb[5];
  Od[1] = d2Rb[6];
  Od[2] = d2Rb[1];
  b_mp[0] = dT[1] * b1c_idx_2 - b1c_idx_1 * dT[2];
  b_mp[1] = d5xpd * dT[2] - dT[0] * b1c_idx_2;
  b_mp[2] = dT[0] * b1c_idx_1 - d5xpd * dT[1];
  b_T[0] = b3c[1] * T[2] - T[1] * b3c[2];
  b_T[1] = T[0] * b3c[2] - b3c[0] * T[2];
  b_T[2] = b3c[0] * T[1] - T[0] * b3c[1];
  d2T[0] = 2.0 * (c_mp[1] * b_dT[2] - b_dT[1] * c_mp[2]);
  d2T[1] = 2.0 * (b_dT[0] * c_mp[2] - c_mp[0] * b_dT[2]);
  d2T[2] = 2.0 * (c_mp[0] * b_dT[1] - b_dT[0] * c_mp[1]);
  for (int i{0}; i < 3; i++) {
    d2Rb[i] = T[i];
    d2Rb[i + 3] = (b_mp[i] + b_T[i]) + d2T[i];
    d2Rb[i + 6] = dT[i];
    for (int i1{0}; i1 < 3; i1++) {
      b_dRb[i + 3 * i1] =
          (dRb[3 * i] * dRb[3 * i1] + dRb[3 * i + 1] * dRb[3 * i1 + 1]) +
          dRb[3 * i + 2] * dRb[3 * i1 + 2];
    }
  }
  r = _mm_loadu_pd(&b_dRb[0]);
  for (int i{0}; i < 3; i++) {
    d = y_tmp[i];
    d1 = y_tmp[i + 3];
    d2 = y_tmp[i + 6];
    for (int i1{0}; i1 < 3; i1++) {
      dRb[i + 3 * i1] =
          (d * d2Rb[3 * i1] + d1 * d2Rb[3 * i1 + 1]) + d2 * d2Rb[3 * i1 + 2];
    }
    aLd[i] = (mq * d2Rb[i] * L_offset[0] + mq * d2Rb[i + 3] * L_offset[1]) +
             mq * d2Rb[i + 6] * L_offset[2];
  }
  r1 = _mm_loadu_pd(&dRb[0]);
  _mm_storeu_pd(&b_dRb[0], _mm_add_pd(r, r1));
  r = _mm_loadu_pd(&b_dRb[2]);
  r1 = _mm_loadu_pd(&dRb[2]);
  _mm_storeu_pd(&b_dRb[2], _mm_add_pd(r, r1));
  r = _mm_loadu_pd(&b_dRb[4]);
  r1 = _mm_loadu_pd(&dRb[4]);
  _mm_storeu_pd(&b_dRb[4], _mm_add_pd(r, r1));
  r = _mm_loadu_pd(&b_dRb[6]);
  r1 = _mm_loadu_pd(&dRb[6]);
  _mm_storeu_pd(&b_dRb[6], _mm_add_pd(r, r1));
  dOd[0] = b_dRb[5];
  dOd[1] = b_dRb[6];
  dOd[2] = b_dRb[1];
  //  output to thrust compensation
}

//
// File trailer for Flatness_mission_spfig8.cpp
//
// [EOF]
//
