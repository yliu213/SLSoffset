//
// File: _coder_Flatness_mission_spfig8_api.h
//
// MATLAB Coder version            : 26.1
// C/C++ source code generated on  : 31-Aug-2026 23:16:44
//

#ifndef _CODER_FLATNESS_MISSION_SPFIG8_API_H
#define _CODER_FLATNESS_MISSION_SPFIG8_API_H

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
void Flatness_mission_spfig8(real_T t, real_T mp, real_T mq, real_T g, real_T l,
                             const real_T L_offset[3], real_T phi, real_T theta,
                             real_T psi, const real_T Omega[3], real_T A,
                             real_T B, real_T w, real_T Od[3], real_T dOd[3],
                             real_T aLd[3], real_T snapd[3]);

void Flatness_mission_spfig8_api(const mxArray *const prhs[13], int32_T nlhs,
                                 const mxArray *plhs[4]);

void Flatness_mission_spfig8_atexit();

void Flatness_mission_spfig8_initialize();

void Flatness_mission_spfig8_terminate();

void Flatness_mission_spfig8_xil_shutdown();

void Flatness_mission_spfig8_xil_terminate();

#endif
//
// File trailer for _coder_Flatness_mission_spfig8_api.h
//
// [EOF]
//
