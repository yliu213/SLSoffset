//
// File: Inner_loop_geometric_PID.h
//
// MATLAB Coder version            : 26.1
// C/C++ source code generated on  : 24-Aug-2026 22:38:47
//

#ifndef INNER_LOOP_GEOMETRIC_PID_H
#define INNER_LOOP_GEOMETRIC_PID_H

// Include Files
#include "rtwtypes.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
extern void Inner_loop_geometric_PID(
    const double rpy_angles[3], const double q[3], const double dq[3],
    const double Omega[3], const double Rd[9], const double Omegad[3],
    const double dOmegad[3], double Fl, const double gains[4],
    const double physics_param[7], const double L_offset[3], const double eI[3],
    double taub[3], double tau[3], double rate_sp_dt1[3], double rate_sp_dt2[3],
    double eI_dt[3]);

#endif
//
// File trailer for Inner_loop_geometric_PID.h
//
// [EOF]
//
