//
// File: QSF_w_offset_intctrl.h
//
// MATLAB Coder version            : 26.1
// C/C++ source code generated on  : 26-Aug-2026 16:37:47
//

#ifndef QSF_W_OFFSET_INTCTRL_H
#define QSF_W_OFFSET_INTCTRL_H

// Include Files
#include "rtwtypes.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
extern void QSF_w_offset_intctrl(double mp, double mq, double l, double g,
                                 const double K1[5], const double K2[5],
                                 const double K3[3], const double ref_traj[15],
                                 const double states[12], double psi,
                                 const double aLd[3], const double Int[3],
                                 double Rbd[9], double *Fld_scaler,
                                 double *phid, double *thetad);

#endif
//
// File trailer for QSF_w_offset_intctrl.h
//
// [EOF]
//
