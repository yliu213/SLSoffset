//
// File: _coder_Flatness_mission_spfig8_mex.cpp
//
// MATLAB Coder version            : 26.1
// C/C++ source code generated on  : 31-Aug-2026 23:16:44
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
                           nullptr, "UTF-8", true);
  return emlrtRootTLSGlobal;
}

//
// Arguments    : int32_T nlhs
//                mxArray *plhs[4]
//                int32_T nrhs
//                const mxArray *prhs[13]
// Return Type  : void
//
void unsafe_Flatness_mission_spfig8_mexFunction(int32_T nlhs, mxArray *plhs[4],
                                                int32_T nrhs,
                                                const mxArray *prhs[13])
{
  emlrtStack st{
      nullptr, // site
      nullptr, // tls
      nullptr  // prev
  };
  const mxArray *outputs[4];
  int32_T i;
  st.tls = emlrtRootTLSGlobal;
  // Check for proper number of arguments.
  if (nrhs != 13) {
    emlrtErrMsgIdAndTxt(&st, "EMLRT:runTime:WrongNumberOfInputs", 5, 12, 13, 4,
                        23, "Flatness_mission_spfig8");
  }
  if (nlhs > 4) {
    emlrtErrMsgIdAndTxt(&st, "EMLRT:runTime:TooManyOutputArguments", 3, 4, 23,
                        "Flatness_mission_spfig8");
  }
  // Call the function.
  Flatness_mission_spfig8_api(prhs, nlhs, outputs);
  // Copy over outputs to the caller.
  if (nlhs < 1) {
    i = 1;
  } else {
    i = nlhs;
  }
  emlrtReturnArrays(i, &plhs[0], &outputs[0]);
}

//
// File trailer for _coder_Flatness_mission_spfig8_mex.cpp
//
// [EOF]
//
