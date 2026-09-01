//
// File: Flatness_mission_spfig8.cpp
//
// MATLAB Coder version            : 26.1
// C/C++ source code generated on  : 31-Aug-2026 23:16:44
//

// Include Files
#include "Flatness_mission_spfig8.h"
#include "norm.h"
#include "rt_nonfinite.h"
#include <cmath>
#include <cstring>
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
  __m128d r10;
  __m128d r11;
  __m128d r12;
  __m128d r13;
  __m128d r2;
  __m128d r3;
  __m128d r4;
  __m128d r5;
  __m128d r6;
  __m128d r7;
  __m128d r8;
  __m128d r9;
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
  double b_u;
  double c_a;
  double c_b;
  double c_du;
  double d;
  double d_a;
  double d_b;
  double d_u;
  double dds;
  double ddu_tmp;
  double e_a;
  double f_a;
  double g_a;
  double h_a;
  double i_a;
  double j_a;
  double s;
  s = coder::b_norm(v);
  r = _mm_set1_pd(s);
  _mm_storeu_pd(&u[0], _mm_div_pd(_mm_loadu_pd(&v[0]), r));
  u[2] = v[2] / s;
  b_u = 0.0;
  a = 1.0 / s;
  std::memset(&du[0], 0, 3U * sizeof(double));
  ddu_tmp = du[0];
  a_tmp = du[1];
  b_du = du[2];
  for (int i{0}; i < 3; i++) {
    int i1;
    b = dv[i];
    b_u += u[i] * b;
    b_a = static_cast<double>(b_I[3 * i]) - u[0] * u[i];
    P[3 * i] = b_a;
    ddu_tmp += b_a * b;
    i1 = 3 * i + 1;
    b_a = static_cast<double>(b_I[i1]) - u[1] * u[i];
    P[i1] = b_a;
    a_tmp += b_a * b;
    i1 = 3 * i + 2;
    b_a = static_cast<double>(b_I[i1]) - u[2] * u[i];
    P[i1] = b_a;
    b_du += b_a * b;
  }
  du[2] = b_du;
  du[1] = a_tmp;
  du[0] = ddu_tmp;
  r1 = _mm_loadu_pd(&du[0]);
  _mm_storeu_pd(&du[0], _mm_mul_pd(_mm_set1_pd(a), r1));
  du[2] *= a;
  ddu_tmp = coder::b_norm(du);
  a_tmp = b_u / s;
  c_a = 2.0 * a_tmp;
  b_b = ddu_tmp * ddu_tmp;
  std::memset(&ddu[0], 0, 3U * sizeof(double));
  r1 = _mm_loadu_pd(&P[0]);
  r2 = _mm_loadu_pd(&ddu[0]);
  r3 = _mm_set1_pd(ddv[0]);
  _mm_storeu_pd(&ddu[0], _mm_add_pd(r2, _mm_mul_pd(r1, r3)));
  ddu[2] += ddv[0] * P[2];
  r1 = _mm_loadu_pd(&P[3]);
  r2 = _mm_loadu_pd(&ddu[0]);
  r4 = _mm_set1_pd(ddv[1]);
  _mm_storeu_pd(&ddu[0], _mm_add_pd(r2, _mm_mul_pd(r1, r4)));
  ddu[2] += ddv[1] * P[5];
  r1 = _mm_loadu_pd(&P[6]);
  r2 = _mm_loadu_pd(&ddu[0]);
  r5 = _mm_set1_pd(ddv[2]);
  _mm_storeu_pd(&ddu[0], _mm_add_pd(r2, _mm_mul_pd(r1, r5)));
  ddu[2] += ddv[2] * P[8];
  ddu[0] = (a * ddu[0] - c_a * du[0]) - u[0] * b_b;
  r1 = _mm_loadu_pd(&du[0]);
  r6 = _mm_set1_pd(u[0]);
  _mm_storeu_pd(&dP[0], _mm_mul_pd(r1, r6));
  r1 = _mm_loadu_pd(&u[0]);
  r7 = _mm_set1_pd(du[0]);
  _mm_storeu_pd(&c_u[0], _mm_mul_pd(r1, r7));
  dP[2] = u[0] * du[2];
  c_u[2] = du[0] * u[2];
  ddu[1] = (a * ddu[1] - c_a * du[1]) - u[1] * b_b;
  r1 = _mm_loadu_pd(&du[0]);
  r8 = _mm_set1_pd(u[1]);
  _mm_storeu_pd(&dP[3], _mm_mul_pd(r1, r8));
  r1 = _mm_loadu_pd(&u[0]);
  r9 = _mm_set1_pd(du[1]);
  _mm_storeu_pd(&c_u[3], _mm_mul_pd(r1, r9));
  dP[5] = u[1] * du[2];
  c_u[5] = du[1] * u[2];
  ddu[2] = (a * ddu[2] - c_a * du[2]) - u[2] * b_b;
  r1 = _mm_loadu_pd(&du[0]);
  r10 = _mm_set1_pd(u[2]);
  _mm_storeu_pd(&dP[6], _mm_mul_pd(r1, r10));
  r1 = _mm_loadu_pd(&u[0]);
  r11 = _mm_set1_pd(du[2]);
  _mm_storeu_pd(&c_u[6], _mm_mul_pd(r1, r11));
  ddu_tmp = du[2] * u[2];
  dP[8] = ddu_tmp;
  c_u[8] = ddu_tmp;
  r1 = _mm_loadu_pd(&dP[0]);
  r2 = _mm_loadu_pd(&c_u[0]);
  r12 = _mm_set1_pd(-1.0);
  _mm_storeu_pd(&dP[0], _mm_mul_pd(_mm_add_pd(r1, r2), r12));
  r1 = _mm_loadu_pd(&dP[2]);
  r2 = _mm_loadu_pd(&c_u[2]);
  _mm_storeu_pd(&dP[2], _mm_mul_pd(_mm_add_pd(r1, r2), r12));
  r1 = _mm_loadu_pd(&dP[4]);
  r2 = _mm_loadu_pd(&c_u[4]);
  _mm_storeu_pd(&dP[4], _mm_mul_pd(_mm_add_pd(r1, r2), r12));
  r1 = _mm_loadu_pd(&dP[6]);
  r2 = _mm_loadu_pd(&c_u[6]);
  _mm_storeu_pd(&dP[6], _mm_mul_pd(_mm_add_pd(r1, r2), r12));
  dP[8] = -(dP[8] + ddu_tmp);
  dds = ((du[0] * dv[0] + du[1] * dv[1]) + du[2] * dv[2]) +
        ((u[0] * ddv[0] + u[1] * ddv[1]) + u[2] * ddv[2]);
  //  Helper scalars
  b_a_tmp = s * s;
  ddu_tmp = b_u * b_u;
  d_a = dds / s - ddu_tmp / b_a_tmp;
  //  db = a;
  //  Third derivative
  r1 = _mm_loadu_pd(&P[0]);
  r2 = _mm_loadu_pd(&dP[0]);
  r13 = _mm_set1_pd(a_tmp);
  _mm_storeu_pd(&d3u_tmp[0], _mm_sub_pd(r2, _mm_mul_pd(r13, r1)));
  r1 = _mm_loadu_pd(&P[2]);
  r2 = _mm_loadu_pd(&dP[2]);
  _mm_storeu_pd(&d3u_tmp[2], _mm_sub_pd(r2, _mm_mul_pd(r13, r1)));
  r1 = _mm_loadu_pd(&P[4]);
  r2 = _mm_loadu_pd(&dP[4]);
  _mm_storeu_pd(&d3u_tmp[4], _mm_sub_pd(r2, _mm_mul_pd(r13, r1)));
  r1 = _mm_loadu_pd(&P[6]);
  r2 = _mm_loadu_pd(&dP[6]);
  _mm_storeu_pd(&d3u_tmp[6], _mm_sub_pd(r2, _mm_mul_pd(r13, r1)));
  d3u_tmp[8] = dP[8] - a_tmp * P[8];
  b_du = (du[0] * ddu[0] + du[1] * ddu[1]) + du[2] * ddu[2];
  e_a = 2.0 * d_a;
  f_a = 2.0 * b_du;
  std::memset(&b_P[0], 0, 3U * sizeof(double));
  r1 = _mm_loadu_pd(&P[0]);
  r2 = _mm_loadu_pd(&b_P[0]);
  _mm_storeu_pd(&b_P[0], _mm_add_pd(r2, _mm_mul_pd(r1, _mm_set1_pd(dddv[0]))));
  b_P[2] += dddv[0] * P[2];
  r1 = _mm_loadu_pd(&P[3]);
  r2 = _mm_loadu_pd(&b_P[0]);
  _mm_storeu_pd(&b_P[0], _mm_add_pd(r2, _mm_mul_pd(r1, _mm_set1_pd(dddv[1]))));
  b_P[2] += dddv[1] * P[5];
  r1 = _mm_loadu_pd(&P[6]);
  r2 = _mm_loadu_pd(&b_P[0]);
  _mm_storeu_pd(&b_P[0], _mm_add_pd(r2, _mm_mul_pd(r1, _mm_set1_pd(dddv[2]))));
  b_P[2] += dddv[2] * P[8];
  //  fourth derivative
  g_a = 2.0 / s;
  c_b = b_u / b_a_tmp;
  h_a = dds / b_a_tmp;
  d_b = ddu_tmp / rt_powd_snf(s, 3.0);
  d_u = 0.0;
  b_ddu = 0.0;
  d = 0.0;
  i_a = 4.0 * d_a;
  j_a = 4.0 * b_du;
  b_du = 0.0;
  c_du = 0.0;
  for (int i{0}; i < 3; i++) {
    ddu_tmp = du[i];
    a_tmp = ddu[i];
    b = u[i];
    b_a = ((((a * b_P[i] +
              ((a * d3u_tmp[i] * ddv[0] + a * d3u_tmp[i + 3] * ddv[1]) +
               a * d3u_tmp[i + 6] * ddv[2])) -
             e_a * ddu_tmp) -
            c_a * a_tmp) -
           f_a * b) -
          ddu_tmp * b_b;
    d3u[i] = b_a;
    d_u += b * dddv[i];
    b_ddu += a_tmp * dv[i];
    d += 2.0 * ddu_tmp * ddv[i];
    b_du += a_tmp * a_tmp;
    c_du += ddu_tmp * b_a;
  }
  __m128d r14;
  b_a = 2.0 * ((((d_u + b_ddu) + d) / s - dds * b_u / b_a_tmp) - c_a * d_a);
  b = b_du + c_du;
  std::memset(&b_P[0], 0, 3U * sizeof(double));
  r1 = _mm_loadu_pd(&P[0]);
  r2 = _mm_loadu_pd(&b_P[0]);
  _mm_storeu_pd(&b_P[0], _mm_add_pd(r2, _mm_mul_pd(r1, _mm_set1_pd(ddddv[0]))));
  r1 = _mm_loadu_pd(&du[0]);
  r2 = _mm_loadu_pd(&ddu[0]);
  r13 = _mm_set1_pd(0.0);
  r14 = _mm_set1_pd(2.0);
  _mm_storeu_pd(&c_ddu[0], _mm_add_pd(_mm_add_pd(r13, _mm_mul_pd(r2, r6)),
                                      _mm_mul_pd(r14, _mm_mul_pd(r1, r7))));
  r1 = _mm_loadu_pd(&u[0]);
  _mm_storeu_pd(&c_u[0], _mm_mul_pd(r1, _mm_set1_pd(ddu[0])));
  b_P[2] += ddddv[0] * P[2];
  c_ddu[2] = u[0] * ddu[2] + 2.0 * (du[0] * du[2]);
  c_u[2] = ddu[0] * u[2];
  r1 = _mm_loadu_pd(&P[3]);
  r2 = _mm_loadu_pd(&b_P[0]);
  _mm_storeu_pd(&b_P[0], _mm_add_pd(r2, _mm_mul_pd(r1, _mm_set1_pd(ddddv[1]))));
  r1 = _mm_loadu_pd(&du[0]);
  r2 = _mm_loadu_pd(&ddu[0]);
  _mm_storeu_pd(&c_ddu[3], _mm_add_pd(_mm_add_pd(r13, _mm_mul_pd(r2, r8)),
                                      _mm_mul_pd(r14, _mm_mul_pd(r1, r9))));
  r1 = _mm_loadu_pd(&u[0]);
  _mm_storeu_pd(&c_u[3], _mm_mul_pd(r1, _mm_set1_pd(ddu[1])));
  b_P[2] += ddddv[1] * P[5];
  c_ddu[5] = u[1] * ddu[2] + 2.0 * (du[1] * du[2]);
  c_u[5] = ddu[1] * u[2];
  r1 = _mm_loadu_pd(&P[6]);
  r2 = _mm_loadu_pd(&b_P[0]);
  _mm_storeu_pd(&b_P[0], _mm_add_pd(r2, _mm_mul_pd(r1, _mm_set1_pd(ddddv[2]))));
  r1 = _mm_loadu_pd(&du[0]);
  r2 = _mm_loadu_pd(&ddu[0]);
  _mm_storeu_pd(&c_ddu[6], _mm_add_pd(_mm_add_pd(r13, _mm_mul_pd(r2, r10)),
                                      _mm_mul_pd(r14, _mm_mul_pd(r1, r11))));
  r1 = _mm_loadu_pd(&u[0]);
  _mm_storeu_pd(&c_u[6], _mm_mul_pd(r1, _mm_set1_pd(ddu[2])));
  b_P[2] += ddddv[2] * P[8];
  ddu_tmp = ddu[2] * u[2];
  c_ddu[8] = ddu_tmp + 2.0 * (du[2] * du[2]);
  c_u[8] = ddu_tmp;
  r1 = _mm_loadu_pd(&dP[0]);
  r2 = _mm_loadu_pd(&c_ddu[0]);
  r6 = _mm_loadu_pd(&c_u[0]);
  r7 = _mm_loadu_pd(&P[0]);
  r11 = _mm_set1_pd(-2.0);
  r13 = _mm_set1_pd(c_b);
  r8 = _mm_set1_pd(h_a);
  r9 = _mm_set1_pd(d_b);
  _mm_storeu_pd(
      &dP[0],
      _mm_add_pd(
          _mm_sub_pd(
              _mm_add_pd(_mm_mul_pd(_mm_mul_pd(r11, r1), r13),
                         _mm_div_pd(_mm_mul_pd(_mm_add_pd(r2, r6), r12), r)),
              _mm_mul_pd(r8, r7)),
          _mm_mul_pd(_mm_mul_pd(r7, r14), r9)));
  r1 = _mm_loadu_pd(&dP[2]);
  r2 = _mm_loadu_pd(&c_ddu[2]);
  r6 = _mm_loadu_pd(&c_u[2]);
  r7 = _mm_loadu_pd(&P[2]);
  _mm_storeu_pd(
      &dP[2],
      _mm_add_pd(
          _mm_sub_pd(
              _mm_add_pd(_mm_mul_pd(_mm_mul_pd(r11, r1), r13),
                         _mm_div_pd(_mm_mul_pd(_mm_add_pd(r2, r6), r12), r)),
              _mm_mul_pd(r8, r7)),
          _mm_mul_pd(_mm_mul_pd(r7, r14), r9)));
  r1 = _mm_loadu_pd(&dP[4]);
  r2 = _mm_loadu_pd(&c_ddu[4]);
  r6 = _mm_loadu_pd(&c_u[4]);
  r7 = _mm_loadu_pd(&P[4]);
  _mm_storeu_pd(
      &dP[4],
      _mm_add_pd(
          _mm_sub_pd(
              _mm_add_pd(_mm_mul_pd(_mm_mul_pd(r11, r1), r13),
                         _mm_div_pd(_mm_mul_pd(_mm_add_pd(r2, r6), r12), r)),
              _mm_mul_pd(r8, r7)),
          _mm_mul_pd(_mm_mul_pd(r7, r14), r9)));
  r1 = _mm_loadu_pd(&dP[6]);
  r2 = _mm_loadu_pd(&c_ddu[6]);
  r6 = _mm_loadu_pd(&c_u[6]);
  r7 = _mm_loadu_pd(&P[6]);
  _mm_storeu_pd(
      &dP[6],
      _mm_add_pd(
          _mm_sub_pd(
              _mm_add_pd(_mm_mul_pd(_mm_mul_pd(r11, r1), r13),
                         _mm_div_pd(_mm_mul_pd(_mm_add_pd(r2, r6), r12), r)),
              _mm_mul_pd(r8, r7)),
          _mm_mul_pd(_mm_mul_pd(r7, r14), r9)));
  dP[8] = ((-2.0 * dP[8] * c_b + -(c_ddu[8] + ddu_tmp) / s) - h_a * P[8]) +
          P[8] * 2.0 * d_b;
  ddu_tmp = dddv[0];
  a_tmp = dddv[1];
  b_du = dddv[2];
  r1 = _mm_loadu_pd(&b_P[0]);
  r13 = _mm_mul_pd(_mm_set1_pd(a), r1);
  r1 = _mm_loadu_pd(&d3u_tmp[0]);
  r1 = _mm_mul_pd(_mm_set1_pd(g_a), r1);
  r2 = _mm_mul_pd(r1, _mm_set1_pd(ddu_tmp));
  r1 = _mm_loadu_pd(&d3u_tmp[3]);
  r1 = _mm_mul_pd(_mm_set1_pd(g_a), r1);
  r1 = _mm_mul_pd(r1, _mm_set1_pd(a_tmp));
  r2 = _mm_add_pd(r2, r1);
  r1 = _mm_loadu_pd(&d3u_tmp[6]);
  r1 = _mm_mul_pd(_mm_set1_pd(g_a), r1);
  r1 = _mm_mul_pd(r1, _mm_set1_pd(b_du));
  r1 = _mm_add_pd(r2, r1);
  r1 = _mm_add_pd(r13, r1);
  _mm_storeu_pd(&d4u[0], r1);
  d4u[2] =
      a * b_P[2] + ((g_a * d3u_tmp[2] * ddu_tmp + g_a * d3u_tmp[5] * a_tmp) +
                    g_a * d3u_tmp[8] * b_du);
  std::memset(&b_P[0], 0, 3U * sizeof(double));
  r1 = _mm_loadu_pd(&dP[0]);
  r2 = _mm_loadu_pd(&b_P[0]);
  _mm_storeu_pd(&b_P[0], _mm_add_pd(r2, _mm_mul_pd(r1, r3)));
  b_P[2] += ddv[0] * dP[2];
  r1 = _mm_loadu_pd(&dP[3]);
  r2 = _mm_loadu_pd(&b_P[0]);
  _mm_storeu_pd(&b_P[0], _mm_add_pd(r2, _mm_mul_pd(r1, r4)));
  b_P[2] += ddv[1] * dP[5];
  r1 = _mm_loadu_pd(&dP[6]);
  r2 = _mm_loadu_pd(&b_P[0]);
  _mm_storeu_pd(&b_P[0], _mm_add_pd(r2, _mm_mul_pd(r1, r5)));
  b_P[2] += ddv[2] * dP[8];
  r1 = _mm_loadu_pd(&d4u[0]);
  r2 = _mm_loadu_pd(&b_P[0]);
  r13 = _mm_loadu_pd(&du[0]);
  r6 = _mm_loadu_pd(&ddu[0]);
  r7 = _mm_loadu_pd(&d3u[0]);
  r11 = _mm_loadu_pd(&u[0]);
  _mm_storeu_pd(
      &d4u[0],
      _mm_sub_pd(
          _mm_sub_pd(
              _mm_sub_pd(
                  _mm_sub_pd(
                      _mm_sub_pd(_mm_sub_pd(_mm_add_pd(r1, r2),
                                            _mm_mul_pd(_mm_set1_pd(b_a), r13)),
                                 _mm_mul_pd(_mm_set1_pd(i_a), r6)),
                      _mm_mul_pd(_mm_set1_pd(c_a), r7)),
                  _mm_mul_pd(r6, _mm_set1_pd(b_b))),
              _mm_mul_pd(_mm_set1_pd(j_a), r13)),
          _mm_mul_pd(_mm_mul_pd(r14, r11), _mm_set1_pd(b))));
  d4u[2] =
      ((((((d4u[2] + b_P[2]) - b_a * du[2]) - i_a * ddu[2]) - c_a * d3u[2]) -
        ddu[2] * b_b) -
       j_a * du[2]) -
      2.0 * u[2] * b;
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
//                double snapd[3]
// Return Type  : void
//
void Flatness_mission_spfig8(double t, double mp, double mq, double g, double l,
                             const double L_offset[3], double phi, double theta,
                             double psi, const double Omega[3], double A,
                             double B, double w, double Od[3], double dOd[3],
                             double aLd[3], double snapd[3])
{
  __m128d r;
  __m128d r1;
  __m128d r2;
  __m128d r3;
  __m128d r4;
  __m128d r5;
  double b_dRb[9];
  double d2Rb[9];
  double dRb[9];
  double y_tmp[9];
  double T[3];
  double b3c[3];
  double b_T[3];
  double b_mp[3];
  double c[3];
  double c_mp[3];
  double d2T[3];
  double d3qd[3];
  double d4qd[3];
  double dT[3];
  double dc[3];
  double dv[3];
  double b1c_idx_0;
  double b1c_idx_2;
  double d;
  double d1;
  double d2;
  double d2xpd;
  double d2ypd;
  double d2ypd_tmp;
  double d3;
  double d3xpd;
  double d3xpd_tmp;
  double d3ypd;
  double d4;
  double d4xpd;
  double d4ypd;
  double d5;
  double d5xpd;
  double d5ypd;
  double d6xpd;
  double d6ypd;
  double thetad;
  int d2Rb_tmp;
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
  if (!(t < 15.0) && !(t < 30.0) && !(t < 45.0) && !(t < 60.0) && (t < 105.0)) {
    //  Figure-8 starts at t = 60 s
    //  position
    //  velocity
    //  acceleration
    thetad = w * (t - 60.0);
    b1c_idx_2 = std::sin(thetad);
    d6ypd = w * w;
    d2xpd = -A * d6ypd * b1c_idx_2;
    d6xpd = 2.0 * w * (t - 60.0);
    d2ypd_tmp = std::sin(d6xpd);
    d2ypd = -2.0 * B * d6ypd * d2ypd_tmp;
    //  jerk
    d3xpd_tmp = std::cos(thetad);
    d6ypd = rt_powd_snf(w, 3.0);
    d3xpd = -A * d6ypd * d3xpd_tmp;
    b1c_idx_0 = std::cos(d6xpd);
    d3ypd = -4.0 * B * d6ypd * b1c_idx_0;
    //  snap
    d6ypd = rt_powd_snf(w, 4.0);
    d4xpd = A * d6ypd * b1c_idx_2;
    d4ypd = 8.0 * B * d6ypd * d2ypd_tmp;
    //  5th derivative
    thetad = rt_powd_snf(w, 5.0);
    d5xpd = A * thetad * d3xpd_tmp;
    d5ypd = 16.0 * B * thetad * b1c_idx_0;
    //  6th derivative
    thetad = rt_powd_snf(w, 6.0);
    d6xpd = -A * thetad * b1c_idx_2;
    d6ypd = -32.0 * B * thetad * d2ypd_tmp;
  }
  //  differential-flatness generator
  //  1) desired tension force vector
  T[0] = mp * d2xpd;
  T[1] = mp * d2ypd;
  d2ypd_tmp = mp * 0.0;
  T[2] = d2ypd_tmp - mp * g;
  dT[0] = mp * d3xpd;
  dT[1] = mp * d3ypd;
  dT[2] = d2ypd_tmp;
  snapd[0] = d4xpd;
  snapd[1] = d4ypd;
  snapd[2] = 0.0;
  //  2) get q
  //  q := p3
  r = _mm_loadu_pd(&snapd[0]);
  r1 = _mm_mul_pd(_mm_set1_pd(mp), r);
  _mm_storeu_pd(&d2T[0], r1);
  r = _mm_loadu_pd(&T[0]);
  r2 = _mm_set1_pd(-1.0);
  _mm_storeu_pd(&b_T[0], _mm_mul_pd(r, r2));
  r = _mm_loadu_pd(&dT[0]);
  _mm_storeu_pd(&dOd[0], _mm_mul_pd(r, r2));
  _mm_storeu_pd(&aLd[0], _mm_mul_pd(r1, r2));
  b_T[2] = -T[2];
  dOd[2] = -d2ypd_tmp;
  aLd[2] = -d2ypd_tmp;
  b_mp[0] = -(mp * d5xpd);
  b_mp[1] = -(mp * d5ypd);
  b_mp[2] = -d2ypd_tmp;
  c_mp[0] = -(mp * d6xpd);
  c_mp[1] = -(mp * d6ypd);
  c_mp[2] = -d2ypd_tmp;
  unitvec_dt(b_T, dOd, aLd, b_mp, c_mp, dc, Od, d3qd, d4qd);
  //  since q = -T/norm(T) := p3
  //  3) find suspension point acc. (p = pl - q*l)
  r = _mm_loadu_pd(&Od[0]);
  r1 = _mm_set1_pd(l);
  _mm_storeu_pd(&Od[0], _mm_mul_pd(r, r1));
  r = _mm_loadu_pd(&d3qd[0]);
  _mm_storeu_pd(&d3qd[0], _mm_mul_pd(r, r1));
  r = _mm_loadu_pd(&d4qd[0]);
  _mm_storeu_pd(&d4qd[0], _mm_mul_pd(r, r1));
  Od[2] *= l;
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
  b_mp[0] = Omega[1] * c[2] - c[1] * Omega[2];
  b_mp[1] = c[0] * Omega[2] - Omega[0] * c[2];
  b_mp[2] = Omega[0] * c[1] - c[0] * Omega[1];
  dOd[0] = d2xpd - Od[0];
  dOd[1] = d2ypd - Od[1];
  dOd[2] = 0.0 - Od[2];
  std::memset(&dv[0], 0, 3U * sizeof(double));
  r = _mm_loadu_pd(&dRb[0]);
  r1 = _mm_loadu_pd(&dv[0]);
  _mm_storeu_pd(&dv[0], _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(b_mp[0]))));
  dv[2] += b_mp[0] * -d1;
  r = _mm_loadu_pd(&dRb[3]);
  r1 = _mm_loadu_pd(&dv[0]);
  _mm_storeu_pd(&dv[0], _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(b_mp[1]))));
  dv[2] += b_mp[1] * dRb[5];
  r = _mm_loadu_pd(&dRb[6]);
  r1 = _mm_loadu_pd(&dv[0]);
  _mm_storeu_pd(&dv[0], _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(b_mp[2]))));
  dv[2] += b_mp[2] * dRb[8];
  b_T[0] = 0.0;
  b_T[1] = 0.0;
  b_T[2] = mq * g;
  r = _mm_loadu_pd(&dOd[0]);
  r1 = _mm_loadu_pd(&dv[0]);
  r3 = _mm_loadu_pd(&T[0]);
  r4 = _mm_loadu_pd(&b_T[0]);
  r5 = _mm_set1_pd(mq);
  _mm_storeu_pd(
      &dOd[0],
      _mm_sub_pd(_mm_add_pd(r3, _mm_mul_pd(r5, _mm_sub_pd(r, r1))), r4));
  dOd[2] = (T[2] + mq * (dOd[2] - dv[2])) - b_T[2];
  thetad = std::atan((dOd[0] * d2 + dOd[1] * d3) / dOd[2]);
  d6ypd = coder::b_norm(dOd);
  d5ypd = std::cos(thetad);
  d6xpd = -std::sin(thetad);
  //  directly by reference
  r = _mm_loadu_pd(&dOd[0]);
  r = _mm_mul_pd(r, r2);
  _mm_storeu_pd(&b3c[0], _mm_div_pd(r, _mm_set1_pd(d6ypd)));
  _mm_storeu_pd(&b_mp[0], r);
  b3c[2] = -dOd[2] / d6ypd;
  b_mp[2] = -dOd[2];
  c[0] = b3c[1] * d6xpd - 0.0 * b3c[2];
  c[1] = d5ypd * b3c[2] - b3c[0] * d6xpd;
  c[2] = b3c[0] * 0.0 - d5ypd * b3c[1];
  d3xpd_tmp = coder::b_norm(c);
  b1c_idx_0 = -(b3c[1] * c[2] - c[1] * b3c[2]) / d3xpd_tmp;
  d5xpd = -(c[0] * b3c[2] - b3c[0] * c[2]) / d3xpd_tmp;
  b1c_idx_2 = -(b3c[0] * c[1] - c[0] * b3c[1]) / d3xpd_tmp;
  dOd[0] = -(dT[0] + mq * (d3xpd - d3qd[0]));
  dOd[1] = -(dT[1] + mq * (d3ypd - d3qd[1]));
  dOd[2] = -(d2ypd_tmp + mq * (0.0 - d3qd[2]));
  aLd[0] = -(d2T[0] + mq * (d4xpd - d4qd[0]));
  aLd[1] = -(d2T[1] + mq * (d4ypd - d4qd[1]));
  aLd[2] = -(d2ypd_tmp + mq * (0.0 - d4qd[2]));
  dv[0] = 0.0;
  b_T[0] = 0.0;
  dv[1] = 0.0;
  b_T[1] = 0.0;
  dv[2] = 0.0;
  b_T[2] = 0.0;
  unitvec_dt(b_mp, dOd, aLd, dv, b_T, c_mp, dT, T, dc);
  //  since b3 = -F/norm(F)
  //  1) assume derivatives of b1d = 0
  //  get 1-4th derivative of b1
  dc[0] = c_mp[1] * d6xpd - 0.0 * c_mp[2];
  dc[1] = d5ypd * c_mp[2] - c_mp[0] * d6xpd;
  dc[2] = c_mp[0] * 0.0 - d5ypd * c_mp[1];
  Od[0] = dT[1] * d6xpd - 0.0 * dT[2];
  Od[1] = d5ypd * dT[2] - dT[0] * d6xpd;
  Od[2] = dT[0] * 0.0 - d5ypd * dT[1];
  d6ypd = ((c[0] * dc[0] + c[1] * dc[1]) + c[2] * dc[2]) / d3xpd_tmp;
  thetad = d6ypd / d3xpd_tmp;
  dOd[0] =
      (-(c_mp[1] * c[2] - c[1] * c_mp[2]) - (b3c[1] * dc[2] - dc[1] * b3c[2])) /
          d3xpd_tmp -
      thetad * b1c_idx_0;
  dOd[1] =
      (-(c[0] * c_mp[2] - c_mp[0] * c[2]) - (dc[0] * b3c[2] - b3c[0] * dc[2])) /
          d3xpd_tmp -
      thetad * d5xpd;
  dOd[2] =
      (-(c_mp[0] * c[1] - c[0] * c_mp[1]) - (b3c[0] * dc[1] - dc[0] * b3c[1])) /
          d3xpd_tmp -
      thetad * b1c_idx_2;
  d6xpd = 2.0 * thetad;
  thetad = ((((dc[0] * dc[0] + dc[1] * dc[1]) + dc[2] * dc[2]) +
             ((c[0] * Od[0] + c[1] * Od[1]) + c[2] * Od[2])) -
            d6ypd * d6ypd) /
           d3xpd_tmp / d3xpd_tmp;
  T[0] = (((-(dT[1] * c[2] - c[1] * dT[2]) -
            2.0 * (c_mp[1] * dc[2] - dc[1] * c_mp[2])) -
           (b3c[1] * Od[2] - Od[1] * b3c[2])) /
              d3xpd_tmp -
          d6xpd * dOd[0]) -
         thetad * b1c_idx_0;
  T[1] = (((-(c[0] * dT[2] - dT[0] * c[2]) -
            2.0 * (dc[0] * c_mp[2] - c_mp[0] * dc[2])) -
           (Od[0] * b3c[2] - b3c[0] * Od[2])) /
              d3xpd_tmp -
          d6xpd * dOd[1]) -
         thetad * d5xpd;
  T[2] = (((-(dT[0] * c[1] - c[0] * dT[1]) -
            2.0 * (c_mp[0] * dc[1] - dc[0] * c_mp[1])) -
           (b3c[0] * Od[1] - Od[0] * b3c[1])) /
              d3xpd_tmp -
          d6xpd * dOd[2]) -
         thetad * b1c_idx_2;
  //  desired Omega
  y_tmp[1] = b3c[1] * b1c_idx_2 - d5xpd * b3c[2];
  y_tmp[4] = b1c_idx_0 * b3c[2] - b3c[0] * b1c_idx_2;
  y_tmp[7] = b3c[0] * d5xpd - b1c_idx_0 * b3c[1];
  dRb[0] = dOd[0];
  dRb[3] = (c_mp[1] * b1c_idx_2 - d5xpd * c_mp[2]) +
           (b3c[1] * dOd[2] - dOd[1] * b3c[2]);
  dRb[6] = c_mp[0];
  y_tmp[0] = b1c_idx_0;
  y_tmp[2] = b3c[0];
  dRb[1] = dOd[1];
  dRb[4] = (b1c_idx_0 * c_mp[2] - c_mp[0] * b1c_idx_2) +
           (dOd[0] * b3c[2] - b3c[0] * dOd[2]);
  dRb[7] = c_mp[1];
  y_tmp[3] = d5xpd;
  y_tmp[5] = b3c[1];
  dRb[2] = dOd[2];
  dRb[5] = (c_mp[0] * d5xpd - b1c_idx_0 * c_mp[1]) +
           (b3c[0] * dOd[1] - dOd[0] * b3c[1]);
  dRb[8] = c_mp[2];
  y_tmp[6] = b1c_idx_2;
  y_tmp[8] = b3c[2];
  std::memset(&d2Rb[0], 0, 9U * sizeof(double));
  for (int i{0}; i < 3; i++) {
    thetad = dRb[3 * i];
    r = _mm_loadu_pd(&y_tmp[0]);
    r1 = _mm_loadu_pd(&d2Rb[3 * i]);
    _mm_storeu_pd(&d2Rb[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(thetad))));
    d2Rb_tmp = 3 * i + 2;
    d2Rb[d2Rb_tmp] += y_tmp[2] * thetad;
    thetad = dRb[3 * i + 1];
    r = _mm_loadu_pd(&y_tmp[3]);
    r1 = _mm_loadu_pd(&d2Rb[3 * i]);
    _mm_storeu_pd(&d2Rb[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(thetad))));
    d2Rb[d2Rb_tmp] += y_tmp[5] * thetad;
    thetad = dRb[d2Rb_tmp];
    r = _mm_loadu_pd(&y_tmp[6]);
    r1 = _mm_loadu_pd(&d2Rb[3 * i]);
    _mm_storeu_pd(&d2Rb[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(thetad))));
    d2Rb[d2Rb_tmp] += y_tmp[8] * thetad;
  }
  Od[0] = d2Rb[5];
  Od[1] = d2Rb[6];
  Od[2] = d2Rb[1];
  b_mp[0] = dT[1] * b1c_idx_2 - d5xpd * dT[2];
  b_mp[1] = b1c_idx_0 * dT[2] - dT[0] * b1c_idx_2;
  b_mp[2] = dT[0] * d5xpd - b1c_idx_0 * dT[1];
  b_T[0] = b3c[1] * T[2] - T[1] * b3c[2];
  b_T[1] = T[0] * b3c[2] - b3c[0] * T[2];
  b_T[2] = b3c[0] * T[1] - T[0] * b3c[1];
  dv[0] = 2.0 * (c_mp[1] * dOd[2] - dOd[1] * c_mp[2]);
  dv[1] = 2.0 * (dOd[0] * c_mp[2] - c_mp[0] * dOd[2]);
  dv[2] = 2.0 * (c_mp[0] * dOd[1] - dOd[0] * c_mp[1]);
  for (int i{0}; i < 3; i++) {
    d2Rb[i] = T[i];
    d2Rb[i + 3] = (b_mp[i] + b_T[i]) + dv[i];
    d2Rb[i + 6] = dT[i];
    for (int i1{0}; i1 < 3; i1++) {
      b_dRb[i1 + 3 * i] =
          (dRb[3 * i1] * dRb[3 * i] + dRb[3 * i1 + 1] * dRb[3 * i + 1]) +
          dRb[3 * i1 + 2] * dRb[3 * i + 2];
    }
  }
  std::memset(&dRb[0], 0, 9U * sizeof(double));
  for (int i{0}; i < 3; i++) {
    thetad = d2Rb[3 * i];
    r = _mm_loadu_pd(&y_tmp[0]);
    r1 = _mm_loadu_pd(&dRb[3 * i]);
    _mm_storeu_pd(&dRb[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(thetad))));
    d2Rb_tmp = 3 * i + 2;
    dRb[d2Rb_tmp] += y_tmp[2] * thetad;
    thetad = d2Rb[3 * i + 1];
    r = _mm_loadu_pd(&y_tmp[3]);
    r1 = _mm_loadu_pd(&dRb[3 * i]);
    _mm_storeu_pd(&dRb[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(thetad))));
    dRb[d2Rb_tmp] += y_tmp[5] * thetad;
    thetad = d2Rb[d2Rb_tmp];
    r = _mm_loadu_pd(&y_tmp[6]);
    r1 = _mm_loadu_pd(&dRb[3 * i]);
    _mm_storeu_pd(&dRb[3 * i],
                  _mm_add_pd(r1, _mm_mul_pd(r, _mm_set1_pd(thetad))));
    dRb[d2Rb_tmp] += y_tmp[8] * thetad;
  }
  r = _mm_loadu_pd(&b_dRb[0]);
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
  std::memset(&aLd[0], 0, 3U * sizeof(double));
  r = _mm_loadu_pd(&d2Rb[0]);
  r1 = _mm_loadu_pd(&aLd[0]);
  _mm_storeu_pd(&aLd[0], _mm_add_pd(r1, _mm_mul_pd(_mm_mul_pd(r5, r),
                                                   _mm_set1_pd(L_offset[0]))));
  aLd[2] += mq * d2Rb[2] * L_offset[0];
  r = _mm_loadu_pd(&d2Rb[3]);
  r1 = _mm_loadu_pd(&aLd[0]);
  _mm_storeu_pd(&aLd[0], _mm_add_pd(r1, _mm_mul_pd(_mm_mul_pd(r5, r),
                                                   _mm_set1_pd(L_offset[1]))));
  aLd[2] += mq * d2Rb[5] * L_offset[1];
  r = _mm_loadu_pd(&d2Rb[6]);
  r1 = _mm_loadu_pd(&aLd[0]);
  _mm_storeu_pd(&aLd[0], _mm_add_pd(r1, _mm_mul_pd(_mm_mul_pd(r5, r),
                                                   _mm_set1_pd(L_offset[2]))));
  aLd[2] += mq * d2Rb[8] * L_offset[2];
  //  output to thrust compensation
}

//
// File trailer for Flatness_mission_spfig8.cpp
//
// [EOF]
//
