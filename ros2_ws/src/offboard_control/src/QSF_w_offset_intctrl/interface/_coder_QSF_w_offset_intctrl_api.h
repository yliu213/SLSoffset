//
// File: _coder_QSF_w_offset_intctrl_api.h
//
// MATLAB Coder version            : 26.1
// C/C++ source code generated on  : 26-Aug-2026 16:37:47
//

#ifndef _CODER_QSF_W_OFFSET_INTCTRL_API_H
#define _CODER_QSF_W_OFFSET_INTCTRL_API_H

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
void QSF_w_offset_intctrl(real_T mp, real_T mq, real_T l, real_T g,
                          const real_T K1[5], const real_T K2[5],
                          const real_T K3[3], const real_T ref_traj[15],
                          const real_T states[12], real_T psi,
                          const real_T aLd[3], const real_T Int[3],
                          real_T Rbd[9], real_T *Fld_scaler, real_T *phid,
                          real_T *thetad);

void QSF_w_offset_intctrl_api(const mxArray *const prhs[12], int32_T nlhs,
                              const mxArray *plhs[4]);

void QSF_w_offset_intctrl_atexit();

void QSF_w_offset_intctrl_initialize();

void QSF_w_offset_intctrl_terminate();

void QSF_w_offset_intctrl_xil_shutdown();

void QSF_w_offset_intctrl_xil_terminate();

#endif
//
// File trailer for _coder_QSF_w_offset_intctrl_api.h
//
// [EOF]
//
