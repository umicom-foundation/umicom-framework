/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/curve_calibration_plan.c
 *
 * PURPOSE:
 *   Govern calibration inputs, tolerance and iteration limits for one curve.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/curve_calibration_plan.h"

#include <string.h>
/*
 * Initialise enterprise curve calibration plan from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_enterprise_curve_calibration_plan_init(UmiEnterpriseCurveCalibrationPlan *p,const char *id,size_t n,double tol,size_t it){ UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||n==0U||it==0U||!umi_quant_number_valid(tol)||tol<=0.0)return UMI_STATUS_INVALID_ARGUMENT; memset(p,0,sizeof *p); s=umi_quant_copy_text(p->curve_id,sizeof p->curve_id,id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; p->instrument_count=n; p->tolerance=tol; p->max_iterations=it; return UMI_STATUS_OK; }
