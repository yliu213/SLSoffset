//
// File: _coder_QSF_w_offset_intctrl_mex.cpp
//
// MATLAB Coder version            : 26.1
// C/C++ source code generated on  : 26-Aug-2026 16:37:47
//

// Include Files
#include "_coder_QSF_w_offset_intctrl_mex.h"
#include "_coder_QSF_w_offset_intctrl_api.h"

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
  mexAtExit(&QSF_w_offset_intctrl_atexit);
  QSF_w_offset_intctrl_initialize();
  unsafe_QSF_w_offset_intctrl_mexFunction(nlhs, plhs, nrhs, prhs);
  QSF_w_offset_intctrl_terminate();
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
//                const mxArray *prhs[12]
// Return Type  : void
//
void unsafe_QSF_w_offset_intctrl_mexFunction(int32_T nlhs, mxArray *plhs[4],
                                             int32_T nrhs,
                                             const mxArray *prhs[12])
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
  if (nrhs != 12) {
    emlrtErrMsgIdAndTxt(&st, "EMLRT:runTime:WrongNumberOfInputs", 5, 12, 12, 4,
                        20, "QSF_w_offset_intctrl");
  }
  if (nlhs > 4) {
    emlrtErrMsgIdAndTxt(&st, "EMLRT:runTime:TooManyOutputArguments", 3, 4, 20,
                        "QSF_w_offset_intctrl");
  }
  // Call the function.
  QSF_w_offset_intctrl_api(prhs, nlhs, outputs);
  // Copy over outputs to the caller.
  if (nlhs < 1) {
    i = 1;
  } else {
    i = nlhs;
  }
  emlrtReturnArrays(i, &plhs[0], &outputs[0]);
}

//
// File trailer for _coder_QSF_w_offset_intctrl_mex.cpp
//
// [EOF]
//
