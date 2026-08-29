//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: _coder_Flatness_mission_spfig8_mex.cpp
//
// MATLAB Coder version            : 24.2
// C/C++ source code generated on  : 27-Aug-2026 15:40:59
//

// Include Files
#include "_coder_Flatness_mission_spfig8_mex.h"
#include "_coder_Flatness_mission_spfig8_api.h"

// Function Definitions
//
// Arguments    : int32_T nlhs
//                mxArray *plhs[]
//                int32_T nrhs
//                const mxArray *prhs[]
// Return Type  : void
//
void mexFunction(int32_T nlhs, mxArray *plhs[], int32_T nrhs,
                 const mxArray *prhs[])
{
  mexAtExit(&Flatness_mission_spfig8_atexit);
  Flatness_mission_spfig8_initialize();
  unsafe_Flatness_mission_spfig8_mexFunction(nlhs, plhs, nrhs, prhs);
  Flatness_mission_spfig8_terminate();
}

//
// Arguments    : void
// Return Type  : emlrtCTX
//
emlrtCTX mexFunctionCreateRootTLS()
{
  emlrtCreateRootTLSR2022a(&emlrtRootTLSGlobal, &emlrtContextGlobal, nullptr, 1,
                           nullptr, "windows-1252", true);
  return emlrtRootTLSGlobal;
}

//
// Arguments    : int32_T nlhs
//                mxArray *plhs[3]
//                int32_T nrhs
//                const mxArray *prhs[13]
// Return Type  : void
//
void unsafe_Flatness_mission_spfig8_mexFunction(int32_T nlhs, mxArray *plhs[3],
                                                int32_T nrhs,
                                                const mxArray *prhs[13])
{
  emlrtStack st{
      nullptr, // site
      nullptr, // tls
      nullptr  // prev
  };
  const mxArray *b_prhs[13];
  const mxArray *outputs[3];
  int32_T i1;
  st.tls = emlrtRootTLSGlobal;
  // Check for proper number of arguments.
  if (nrhs != 13) {
    emlrtErrMsgIdAndTxt(&st, "EMLRT:runTime:WrongNumberOfInputs", 5, 12, 13, 4,
                        23, "Flatness_mission_spfig8");
  }
  if (nlhs > 3) {
    emlrtErrMsgIdAndTxt(&st, "EMLRT:runTime:TooManyOutputArguments", 3, 4, 23,
                        "Flatness_mission_spfig8");
  }
  // Call the function.
  for (int32_T i{0}; i < 13; i++) {
    b_prhs[i] = prhs[i];
  }
  Flatness_mission_spfig8_api(b_prhs, nlhs, outputs);
  // Copy over outputs to the caller.
  if (nlhs < 1) {
    i1 = 1;
  } else {
    i1 = nlhs;
  }
  emlrtReturnArrays(i1, &plhs[0], &outputs[0]);
}

//
// File trailer for _coder_Flatness_mission_spfig8_mex.cpp
//
// [EOF]
//
