//
// File: _coder_Flatness_mission_spfig8_api.cpp
//
// MATLAB Coder version            : 26.1
// C/C++ source code generated on  : 31-Aug-2026 23:16:44
//

// Include Files
#include "_coder_Flatness_mission_spfig8_api.h"
#include "_coder_Flatness_mission_spfig8_mex.h"

// Variable Definitions
emlrtCTX emlrtRootTLSGlobal{nullptr};

emlrtContext emlrtContextGlobal{
    true,                                                 // bFirstTime
    false,                                                // bInitialized
    131690U,                                              // fVersionInfo
    nullptr,                                              // fErrorFunction
    "Flatness_mission_spfig8",                            // fFunctionName
    nullptr,                                              // fRTCallStack
    false,                                                // bDebugMode
    {2045744189U, 2170104910U, 2743257031U, 4284093946U}, // fSigWrd
    nullptr                                               // fSigMem
};

// Function Declarations
static real_T (*b_emlrt_marshallIn(const emlrtStack &sp,
                                   const mxArray *b_nullptr,
                                   const char_T *identifier))[3];

static real_T (*b_emlrt_marshallIn(const emlrtStack &sp, const mxArray *u,
                                   const emlrtMsgIdentifier *parentId))[3];

static real_T c_emlrt_marshallIn(const emlrtStack &sp, const mxArray *src,
                                 const emlrtMsgIdentifier *msgId);

static real_T (*d_emlrt_marshallIn(const emlrtStack &sp, const mxArray *src,
                                   const emlrtMsgIdentifier *msgId))[3];

static void emlrtExitTimeCleanupDtorFcn(const void *r);

static real_T emlrt_marshallIn(const emlrtStack &sp, const mxArray *b_nullptr,
                               const char_T *identifier);

static real_T emlrt_marshallIn(const emlrtStack &sp, const mxArray *u,
                               const emlrtMsgIdentifier *parentId);

static const mxArray *emlrt_marshallOut(real_T u[3]);

// Function Definitions
//
// Arguments    : const emlrtStack &sp
//                const mxArray *b_nullptr
//                const char_T *identifier
// Return Type  : real_T (*)[3]
//
static real_T (*b_emlrt_marshallIn(const emlrtStack &sp,
                                   const mxArray *b_nullptr,
                                   const char_T *identifier))[3]
{
  emlrtMsgIdentifier thisId;
  real_T(*y)[3];
  thisId.fIdentifier = const_cast<const char_T *>(identifier);
  thisId.fParent = nullptr;
  thisId.bParentIsCell = false;
  y = b_emlrt_marshallIn(sp, emlrtAlias(b_nullptr), &thisId);
  emlrtDestroyArray(&b_nullptr);
  return y;
}

//
// Arguments    : const emlrtStack &sp
//                const mxArray *u
//                const emlrtMsgIdentifier *parentId
// Return Type  : real_T (*)[3]
//
static real_T (*b_emlrt_marshallIn(const emlrtStack &sp, const mxArray *u,
                                   const emlrtMsgIdentifier *parentId))[3]
{
  real_T(*y)[3];
  y = d_emlrt_marshallIn(sp, emlrtAlias(u), parentId);
  emlrtDestroyArray(&u);
  return y;
}

//
// Arguments    : const emlrtStack &sp
//                const mxArray *src
//                const emlrtMsgIdentifier *msgId
// Return Type  : real_T
//
static real_T c_emlrt_marshallIn(const emlrtStack &sp, const mxArray *src,
                                 const emlrtMsgIdentifier *msgId)
{
  static const int32_T dims{0};
  real_T ret;
  emlrtCheckBuiltInR2012b((emlrtConstCTX)&sp, msgId, src, "double", false, 0U,
                          (const void *)&dims);
  ret = *static_cast<real_T *>(emlrtMxGetData(src));
  emlrtDestroyArray(&src);
  return ret;
}

//
// Arguments    : const emlrtStack &sp
//                const mxArray *src
//                const emlrtMsgIdentifier *msgId
// Return Type  : real_T (*)[3]
//
static real_T (*d_emlrt_marshallIn(const emlrtStack &sp, const mxArray *src,
                                   const emlrtMsgIdentifier *msgId))[3]
{
  static const int32_T dims{3};
  real_T(*ret)[3];
  int32_T i;
  boolean_T b{false};
  emlrtCheckVsBuiltInR2012b((emlrtConstCTX)&sp, msgId, src, "double", false, 1U,
                            (const void *)&dims, &b, &i);
  ret = (real_T(*)[3])emlrtMxGetData(src);
  emlrtDestroyArray(&src);
  return ret;
}

//
// Arguments    : const void *r
// Return Type  : void
//
static void emlrtExitTimeCleanupDtorFcn(const void *r)
{
  emlrtExitTimeCleanup(&emlrtContextGlobal);
}

//
// Arguments    : const emlrtStack &sp
//                const mxArray *b_nullptr
//                const char_T *identifier
// Return Type  : real_T
//
static real_T emlrt_marshallIn(const emlrtStack &sp, const mxArray *b_nullptr,
                               const char_T *identifier)
{
  emlrtMsgIdentifier thisId;
  real_T y;
  thisId.fIdentifier = const_cast<const char_T *>(identifier);
  thisId.fParent = nullptr;
  thisId.bParentIsCell = false;
  y = emlrt_marshallIn(sp, emlrtAlias(b_nullptr), &thisId);
  emlrtDestroyArray(&b_nullptr);
  return y;
}

//
// Arguments    : const emlrtStack &sp
//                const mxArray *u
//                const emlrtMsgIdentifier *parentId
// Return Type  : real_T
//
static real_T emlrt_marshallIn(const emlrtStack &sp, const mxArray *u,
                               const emlrtMsgIdentifier *parentId)
{
  real_T y;
  y = c_emlrt_marshallIn(sp, emlrtAlias(u), parentId);
  emlrtDestroyArray(&u);
  return y;
}

//
// Arguments    : real_T u[3]
// Return Type  : const mxArray *
//
static const mxArray *emlrt_marshallOut(real_T u[3])
{
  static const int32_T i{0};
  static const int32_T i1{3};
  const mxArray *m;
  const mxArray *y;
  void *existingData;
  y = nullptr;
  m = emlrtCreateNumericArray(1, (const void *)&i, mxDOUBLE_CLASS, mxREAL);
  existingData = emlrtMxGetData((mxArray *)m);
  if (existingData != (void *)&u[0]) {
    emlrtFreeMex(existingData);
  }
  emlrtMxSetData((mxArray *)m, &u[0]);
  emlrtSetDimensions((mxArray *)m, &i1, 1);
  emlrtAssign(&y, m);
  return y;
}

//
// Arguments    : const mxArray * const prhs[13]
//                int32_T nlhs
//                const mxArray *plhs[4]
// Return Type  : void
//
void Flatness_mission_spfig8_api(const mxArray *const prhs[13], int32_T nlhs,
                                 const mxArray *plhs[4])
{
  emlrtStack st{
      nullptr, // site
      nullptr, // tls
      nullptr  // prev
  };
  real_T(*L_offset)[3];
  real_T(*Od)[3];
  real_T(*Omega)[3];
  real_T(*aLd)[3];
  real_T(*dOd)[3];
  real_T(*snapd)[3];
  real_T A;
  real_T B;
  real_T g;
  real_T l;
  real_T mp;
  real_T mq;
  real_T phi;
  real_T psi;
  real_T t;
  real_T theta;
  real_T w;
  st.tls = emlrtRootTLSGlobal;
  Od = (real_T(*)[3])mxMalloc(sizeof(real_T[3]));
  dOd = (real_T(*)[3])mxMalloc(sizeof(real_T[3]));
  aLd = (real_T(*)[3])mxMalloc(sizeof(real_T[3]));
  snapd = (real_T(*)[3])mxMalloc(sizeof(real_T[3]));
  // Marshall function inputs
  t = emlrt_marshallIn(st, emlrtAliasP(prhs[0]), "t");
  mp = emlrt_marshallIn(st, emlrtAliasP(prhs[1]), "mp");
  mq = emlrt_marshallIn(st, emlrtAliasP(prhs[2]), "mq");
  g = emlrt_marshallIn(st, emlrtAliasP(prhs[3]), "g");
  l = emlrt_marshallIn(st, emlrtAliasP(prhs[4]), "l");
  L_offset = b_emlrt_marshallIn(st, emlrtAlias(prhs[5]), "L_offset");
  phi = emlrt_marshallIn(st, emlrtAliasP(prhs[6]), "phi");
  theta = emlrt_marshallIn(st, emlrtAliasP(prhs[7]), "theta");
  psi = emlrt_marshallIn(st, emlrtAliasP(prhs[8]), "psi");
  Omega = b_emlrt_marshallIn(st, emlrtAlias(prhs[9]), "Omega");
  A = emlrt_marshallIn(st, emlrtAliasP(prhs[10]), "A");
  B = emlrt_marshallIn(st, emlrtAliasP(prhs[11]), "B");
  w = emlrt_marshallIn(st, emlrtAliasP(prhs[12]), "w");
  // Invoke the target function
  Flatness_mission_spfig8(t, mp, mq, g, l, *L_offset, phi, theta, psi, *Omega,
                          A, B, w, *Od, *dOd, *aLd, *snapd);
  // Marshall function outputs
  plhs[0] = emlrt_marshallOut(*Od);
  if (nlhs > 1) {
    plhs[1] = emlrt_marshallOut(*dOd);
  }
  if (nlhs > 2) {
    plhs[2] = emlrt_marshallOut(*aLd);
  }
  if (nlhs > 3) {
    plhs[3] = emlrt_marshallOut(*snapd);
  }
}

//
// Arguments    : void
// Return Type  : void
//
void Flatness_mission_spfig8_atexit()
{
  emlrtStack st{
      nullptr, // site
      nullptr, // tls
      nullptr  // prev
  };
  mexFunctionCreateRootTLS();
  st.tls = emlrtRootTLSGlobal;
  emlrtPushHeapReferenceStackR2021a(&st, false, nullptr,
                                    (void *)&emlrtExitTimeCleanupDtorFcn,
                                    nullptr, nullptr, nullptr);
  emlrtEnterRtStackR2012b(&st);
  emlrtDestroyRootTLS(&emlrtRootTLSGlobal);
  Flatness_mission_spfig8_xil_terminate();
  Flatness_mission_spfig8_xil_shutdown();
  emlrtExitTimeCleanup(&emlrtContextGlobal);
}

//
// Arguments    : void
// Return Type  : void
//
void Flatness_mission_spfig8_initialize()
{
  emlrtStack st{
      nullptr, // site
      nullptr, // tls
      nullptr  // prev
  };
  mexFunctionCreateRootTLS();
  st.tls = emlrtRootTLSGlobal;
  emlrtClearAllocCountR2012b(&st, false, 0U, nullptr);
  emlrtEnterRtStackR2012b(&st);
  emlrtFirstTimeR2012b(emlrtRootTLSGlobal);
}

//
// Arguments    : void
// Return Type  : void
//
void Flatness_mission_spfig8_terminate()
{
  emlrtDestroyRootTLS(&emlrtRootTLSGlobal);
}

//
// File trailer for _coder_Flatness_mission_spfig8_api.cpp
//
// [EOF]
//
