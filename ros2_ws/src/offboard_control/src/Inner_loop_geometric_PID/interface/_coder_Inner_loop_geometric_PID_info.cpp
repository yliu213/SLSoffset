//
// File: _coder_Inner_loop_geometric_PID_info.cpp
//
// MATLAB Coder version            : 26.1
// C/C++ source code generated on  : 24-Aug-2026 22:38:47
//

// Include Files
#include "_coder_Inner_loop_geometric_PID_info.h"
#include "emlrt.h"
#include "tmwtypes.h"

// Function Declarations
static const mxArray *emlrtMexFcnResolvedFunctionsInfo();

// Function Definitions
//
// Arguments    : void
// Return Type  : const mxArray *
//
static const mxArray *emlrtMexFcnResolvedFunctionsInfo()
{
  const mxArray *nameCaptureInfo;
  const char_T *data[6]{
      "789ced55cd4edb40101e5b2104899f70411c38201e8070402ddc08a1a8919a8248549010"
      "0a212c60f0cfca71023c44558e3df6c891638f7d841e7be15dea608f"
      "638f3c6cf847829146e3f1b73bdf78d63b035ab9a201c0280472910dec48e8e743ab4352"
      "28ae3116650032897d88ff086dd3b13d71e6058eddb044b4d3b065db",
      "ab9d4bd1f21d57b41cb323f66f9003c31435c312d5b8f3b5eb59ab312872ba50f7b97424"
      "9a27d5b605ee51ab97a11977a27aec32df9b51d4830aad075d877c67"
      "f7e4c3f8930a3ec4cbb62ddcbae938b27e281c4b78aed1acaf9757308f7f0ccf38894b79"
      "b430d34a69ad1af8c10fd5f209ec43f577a20f0c9e25bc941ff1c9de",
      "ab6b3d5c1757cce327c3d36fbde7993cf204dffeb453a8146b5f8acbd32baed11185e000"
      "3af305ee24662d00f57f38d6679e5cbd4620776337a6be1f3f271fca"
      "5be17be8bd9e60f8f20497df164ebd0fedb993eac7a2b75adb70f7168bf6e75e1eeb0a1e"
      "551ec0f8cf15ff6f2e7dff509ff1652e3dbe4e7020737169303dee63",
      "cdc52182a38f7d1405f3b962e6f4ddfb73324e773e6eba0d29fd9e548ff12d317caa7355"
      "f56b8c3fada5ef579deb1f457c9daccb84ef167cddf255fafacbd7cb"
      "985ef9dab0cfa399c5093d7be447bd4d34e6396dddef983f4a70f42553bf7efbd90c934b"
      "b45e4be23a593f1cdaec13db7d70a00d7b60827815cf51df63eaafba",
      "1fb48edcfd908afd5c7c555fc5fef3daf97799f8c39094bbd6778099d7efe7f9b4fcb97b"
      "f67b8c77c9c4d7094ee7f80513f7a5e638ca7f642098f9",
      ""};
  nameCaptureInfo = nullptr;
  emlrtNameCaptureMxArrayR2016a(&data[0], 3976U, &nameCaptureInfo);
  return nameCaptureInfo;
}

//
// Arguments    : void
// Return Type  : mxArray *
//
mxArray *emlrtMexFcnProperties()
{
  mxArray *xEntryPoints;
  mxArray *xInputs;
  mxArray *xResult;
  const char_T *epFieldName[7]{
      "QualifiedName",    "NumberOfInputs", "NumberOfOutputs", "ConstantInputs",
      "ResolvedFilePath", "TimeStamp",      "Visible"};
  const char_T *propFieldName[7]{
      "Version",      "ResolvedFunctions", "Checksum", "EntryPoints",
      "CoverageInfo", "IsPolymorphic",     "AuxData"};
  uint8_T v[216]{
      0U,   1U,   73U,  77U,  0U,   0U,   0U,   0U,   14U,  0U,   0U,   0U,
      200U, 0U,   0U,   0U,   6U,   0U,   0U,   0U,   8U,   0U,   0U,   0U,
      2U,   0U,   0U,   0U,   0U,   0U,   0U,   0U,   5U,   0U,   0U,   0U,
      8U,   0U,   0U,   0U,   1U,   0U,   0U,   0U,   1U,   0U,   0U,   0U,
      1U,   0U,   0U,   0U,   0U,   0U,   0U,   0U,   5U,   0U,   4U,   0U,
      17U,  0U,   0U,   0U,   1U,   0U,   0U,   0U,   17U,  0U,   0U,   0U,
      67U,  108U, 97U,  115U, 115U, 69U,  110U, 116U, 114U, 121U, 80U,  111U,
      105U, 110U, 116U, 115U, 0U,   0U,   0U,   0U,   0U,   0U,   0U,   0U,
      14U,  0U,   0U,   0U,   112U, 0U,   0U,   0U,   6U,   0U,   0U,   0U,
      8U,   0U,   0U,   0U,   2U,   0U,   0U,   0U,   0U,   0U,   0U,   0U,
      5U,   0U,   0U,   0U,   8U,   0U,   0U,   0U,   1U,   0U,   0U,   0U,
      0U,   0U,   0U,   0U,   1U,   0U,   0U,   0U,   0U,   0U,   0U,   0U,
      5U,   0U,   4U,   0U,   14U,  0U,   0U,   0U,   1U,   0U,   0U,   0U,
      56U,  0U,   0U,   0U,   81U,  117U, 97U,  108U, 105U, 102U, 105U, 101U,
      100U, 78U,  97U,  109U, 101U, 0U,   77U,  101U, 116U, 104U, 111U, 100U,
      115U, 0U,   0U,   0U,   0U,   0U,   0U,   0U,   80U,  114U, 111U, 112U,
      101U, 114U, 116U, 105U, 101U, 115U, 0U,   0U,   0U,   0U,   72U,  97U,
      110U, 100U, 108U, 101U, 0U,   0U,   0U,   0U,   0U,   0U,   0U,   0U};
  xEntryPoints =
      emlrtCreateStructMatrix(1, 1, 7, (const char_T **)&epFieldName[0]);
  xInputs = emlrtCreateLogicalMatrix(1, 12);
  emlrtSetField(xEntryPoints, 0, "QualifiedName",
                emlrtMxCreateString("Inner_loop_geometric_PID"));
  emlrtSetField(xEntryPoints, 0, "NumberOfInputs",
                emlrtMxCreateDoubleScalar(12.0));
  emlrtSetField(xEntryPoints, 0, "NumberOfOutputs",
                emlrtMxCreateDoubleScalar(5.0));
  emlrtSetField(xEntryPoints, 0, "ConstantInputs", xInputs);
  emlrtSetField(
      xEntryPoints, 0, "ResolvedFilePath",
      emlrtMxCreateString("/MATLAB Drive/Inner_v4/Inner_loop_geometric_PID.m"));
  emlrtSetField(xEntryPoints, 0, "TimeStamp",
                emlrtMxCreateDoubleScalar(740219.1935416666));
  emlrtSetField(xEntryPoints, 0, "Visible", emlrtMxCreateLogicalScalar(true));
  xResult =
      emlrtCreateStructMatrix(1, 1, 7, (const char_T **)&propFieldName[0]);
  emlrtSetField(xResult, 0, "Version",
                emlrtMxCreateString("26.1.0.3346908 (R2026a) Update 5"));
  emlrtSetField(xResult, 0, "ResolvedFunctions",
                (mxArray *)emlrtMexFcnResolvedFunctionsInfo());
  emlrtSetField(xResult, 0, "Checksum",
                emlrtMxCreateString("n2LPYtoYlY4MHhq0w6U8sD"));
  emlrtSetField(xResult, 0, "EntryPoints", xEntryPoints);
  emlrtSetField(xResult, 0, "AuxData",
                emlrtMxCreateRowVectorUINT8((const uint8_T *)&v, 216U));
  return xResult;
}

//
// File trailer for _coder_Inner_loop_geometric_PID_info.cpp
//
// [EOF]
//
