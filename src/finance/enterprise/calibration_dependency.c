/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/calibration_dependency.c
 *
 * PURPOSE:
 *   Describe ordering dependencies between calibration jobs.
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

#include "umicom/finance/enterprise/calibration_dependency.h"

#include <string.h>
/*
 * Initialise enterprise calibration dependency from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_enterprise_calibration_dependency_init(UmiEnterpriseCalibrationDependency *d,const char *a,const char *b){ UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(d==NULL||a==NULL||b==NULL||strcmp(a,b)==0)return UMI_STATUS_INVALID_ARGUMENT; memset(d,0,sizeof *d); s=umi_quant_copy_text(d->prerequisite_id,sizeof d->prerequisite_id,a); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; return umi_quant_copy_text(d->dependent_id,sizeof d->dependent_id,b); }
