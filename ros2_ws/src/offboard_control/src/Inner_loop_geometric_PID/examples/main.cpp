//
// File: main.cpp
//
// MATLAB Coder version            : 26.1
// C/C++ source code generated on  : 24-Aug-2026 22:38:47
//

/*************************************************************************/
/* This automatically generated example C++ main file shows how to call  */
/* entry-point functions that MATLAB Coder generated. You must customize */
/* this file for your application. Do not modify this file directly.     */
/* Instead, make a copy of this file, modify it, and integrate it into   */
/* your development environment.                                         */
/*                                                                       */
/* This file initializes entry-point function arguments to a default     */
/* size and value before calling the entry-point functions. It does      */
/* not store or use any values returned from the entry-point functions.  */
/* If necessary, it does pre-allocate memory for returned values.        */
/* You can use this file as a starting point for a main function that    */
/* you can deploy in your application.                                   */
/*                                                                       */
/* After you copy the file, and before you deploy it, you must make the  */
/* following changes:                                                    */
/* * For variable-size function arguments, change the example sizes to   */
/* the sizes that your application requires.                             */
/* * Change the example values of function arguments to the values that  */
/* your application requires.                                            */
/* * If the entry-point functions return values, store these values or   */
/* otherwise use them as required by your application.                   */
/*                                                                       */
/*************************************************************************/

// Include Files
#include "main.h"
#include "Inner_loop_geometric_PID.h"
#include "Inner_loop_geometric_PID_initialize.h"
#include "Inner_loop_geometric_PID_terminate.h"
#include "rt_nonfinite.h"

// Function Declarations
static void argInit_3x1_real_T(double result[3]);

static void argInit_3x3_real_T(double result[9]);

static void argInit_4x1_real_T(double result[4]);

static void argInit_7x1_real_T(double result[7]);

static double argInit_real_T();

// Function Definitions
//
// Arguments    : double result[3]
// Return Type  : void
//
static void argInit_3x1_real_T(double result[3])
{
  // Loop over the array to initialize each element.
  for (int idx0{0}; idx0 < 3; idx0++) {
    // Set the value of the array element.
    // Change this value to the value that the application requires.
    result[idx0] = argInit_real_T();
  }
}

//
// Arguments    : double result[9]
// Return Type  : void
//
static void argInit_3x3_real_T(double result[9])
{
  // Loop over the array to initialize each element.
  for (int i{0}; i < 9; i++) {
    // Set the value of the array element.
    // Change this value to the value that the application requires.
    result[i] = argInit_real_T();
  }
}

//
// Arguments    : double result[4]
// Return Type  : void
//
static void argInit_4x1_real_T(double result[4])
{
  // Loop over the array to initialize each element.
  for (int idx0{0}; idx0 < 4; idx0++) {
    // Set the value of the array element.
    // Change this value to the value that the application requires.
    result[idx0] = argInit_real_T();
  }
}

//
// Arguments    : double result[7]
// Return Type  : void
//
static void argInit_7x1_real_T(double result[7])
{
  // Loop over the array to initialize each element.
  for (int idx0{0}; idx0 < 7; idx0++) {
    // Set the value of the array element.
    // Change this value to the value that the application requires.
    result[idx0] = argInit_real_T();
  }
}

//
// Arguments    : void
// Return Type  : double
//
static double argInit_real_T()
{
  return 0.0;
}

//
// Arguments    : int argc
//                char **argv
// Return Type  : int
//
int main(int, char **)
{
  // Initialize the application.
  // You do not need to do this more than one time.
  Inner_loop_geometric_PID_initialize();
  // Invoke the entry-point functions.
  // You can call entry-point functions multiple times.
  main_Inner_loop_geometric_PID();
  // Terminate the application.
  // You do not need to do this more than one time.
  Inner_loop_geometric_PID_terminate();
  return 0;
}

//
// Arguments    : void
// Return Type  : void
//
void main_Inner_loop_geometric_PID()
{
  double dv[9];
  double dv2[7];
  double dv1[4];
  double eI_dt[3];
  double rate_sp_dt1[3];
  double rate_sp_dt2[3];
  double rpy_angles_tmp[3];
  double tau[3];
  double taub[3];
  // Initialize function 'Inner_loop_geometric_PID' input arguments.
  // Initialize function input argument 'rpy_angles'.
  argInit_3x1_real_T(rpy_angles_tmp);
  // Initialize function input argument 'q'.
  // Initialize function input argument 'dq'.
  // Initialize function input argument 'Omega'.
  // Initialize function input argument 'Rd'.
  // Initialize function input argument 'Omegad'.
  // Initialize function input argument 'dOmegad'.
  // Initialize function input argument 'gains'.
  // Initialize function input argument 'physics_param'.
  // Initialize function input argument 'L_offset'.
  // Initialize function input argument 'eI'.
  // Call the entry-point 'Inner_loop_geometric_PID'.
  argInit_3x3_real_T(dv);
  argInit_4x1_real_T(dv1);
  argInit_7x1_real_T(dv2);
  Inner_loop_geometric_PID(rpy_angles_tmp, rpy_angles_tmp, rpy_angles_tmp,
                           rpy_angles_tmp, dv, rpy_angles_tmp, rpy_angles_tmp,
                           argInit_real_T(), dv1, dv2, rpy_angles_tmp,
                           rpy_angles_tmp, taub, tau, rate_sp_dt1, rate_sp_dt2,
                           eI_dt);
}

//
// File trailer for main.cpp
//
// [EOF]
//
