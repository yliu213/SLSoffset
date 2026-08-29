//
// File: _coder_Inner_loop_geometric_PID_api.h
//
// MATLAB Coder version            : 26.1
// C/C++ source code generated on  : 24-Aug-2026 22:38:47
//

#ifndef _CODER_INNER_LOOP_GEOMETRIC_PID_API_H
#define _CODER_INNER_LOOP_GEOMETRIC_PID_API_H

// Include Files
#include "emlrt.h"
#include "mex.h"
#include "tmwtypes.h"
#include <algorithm>
#include <cstring>

// Variable Declarations
extern emlrtCTX emlrtRootTLSGlobal;
extern emlrtContext emlrtContextGlobal;

// Function Declarations
void Inner_loop_geometric_PID(
    const real_T rpy_angles[3], const real_T q[3], const real_T dq[3],
    const real_T Omega[3], const real_T Rd[9], const real_T Omegad[3],
    const real_T dOmegad[3], real_T Fl, const real_T gains[4],
    const real_T physics_param[7], const real_T L_offset[3], const real_T eI[3],
    real_T taub[3], real_T tau[3], real_T rate_sp_dt1[3], real_T rate_sp_dt2[3],
    real_T eI_dt[3]);

void Inner_loop_geometric_PID_api(const mxArray *const prhs[12], int32_T nlhs,
                                  const mxArray *plhs[5]);

void Inner_loop_geometric_PID_atexit();

void Inner_loop_geometric_PID_initialize();

void Inner_loop_geometric_PID_terminate();

void Inner_loop_geometric_PID_xil_shutdown();

void Inner_loop_geometric_PID_xil_terminate();

#endif
//
// File trailer for _coder_Inner_loop_geometric_PID_api.h
//
// [EOF]
//
