/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/surface_calibration_plan.c
 *
 * PURPOSE:
 *   Govern calibration inputs, tolerance and iteration limits for one volatility surface.
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

#include "umicom/finance/enterprise/surface_calibration_plan.h"

#include <string.h>
UmiStatus umi_enterprise_surface_calibration_plan_init(UmiEnterpriseSurfaceCalibrationPlan *p,const char *id,size_t n,double tol,size_t it){ UmiStatus s; if(p==NULL||n<3U||it==0U||!umi_quant_number_valid(tol)||tol<=0.0)return UMI_STATUS_INVALID_ARGUMENT; memset(p,0,sizeof *p); s=umi_quant_copy_text(p->surface_id,sizeof p->surface_id,id); if(s!=UMI_STATUS_OK)return s; p->point_count=n; p->tolerance=tol; p->max_iterations=it; return UMI_STATUS_OK; }
