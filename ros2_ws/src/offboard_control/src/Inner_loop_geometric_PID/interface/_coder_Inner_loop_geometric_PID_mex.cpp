//
// File: _coder_Inner_loop_geometric_PID_mex.cpp
//
// MATLAB Coder version            : 26.1
// C/C++ source code generated on  : 24-Aug-2026 22:38:47
//

// Include Files
#include "_coder_Inner_loop_geometric_PID_mex.h"
#include "_coder_Inner_loop_geometric_PID_api.h"

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
  mexAtExit(&Inner_loop_geometric_PID_atexit);
  Inner_loop_geometric_PID_initialize();
  unsafe_Inner_loop_geometric_PID_mexFunction(nlhs, plhs, nrhs, prhs);
  Inner_loop_geometric_PID_terminate();
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
//                mxArray *plhs[5]
//                int32_T nrhs
//                const mxArray *prhs[12]
// Return Type  : void
//
void unsafe_Inner_loop_geometric_PID_mexFunction(int32_T nlhs, mxArray *plhs[5],
                                                 int32_T nrhs,
                                                 const mxArray *prhs[12])
{
  emlrtStack st{
      nullptr, // site
      nullptr, // tls
      nullptr  // prev
  };
  const mxArray *outputs[5];
  int32_T i;
  st.tls = emlrtRootTLSGlobal;
  // Check for proper number of arguments.
  if (nrhs != 12) {
    emlrtErrMsgIdAndTxt(&st, "EMLRT:runTime:WrongNumberOfInputs", 5, 12, 12, 4,
                        24, "Inner_loop_geometric_PID");
  }
  if (nlhs > 5) {
    emlrtErrMsgIdAndTxt(&st, "EMLRT:runTime:TooManyOutputArguments", 3, 4, 24,
                        "Inner_loop_geometric_PID");
  }
  // Call the function.
  Inner_loop_geometric_PID_api(prhs, nlhs, outputs);
  // Copy over outputs to the caller.
  if (nlhs < 1) {
    i = 1;
  } else {
    i = nlhs;
  }
  emlrtReturnArrays(i, &plhs[0], &outputs[0]);
}

//
// File trailer for _coder_Inner_loop_geometric_PID_mex.cpp
//
// [EOF]
//
