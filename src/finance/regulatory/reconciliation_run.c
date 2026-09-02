/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/reconciliation_run.c
 *
 * PURPOSE:
 *   Aggregate matched and broken reconciliation items with total difference.
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

#include "umicom/finance/regulatory/reconciliation_run.h"

#include <math.h>
#include <string.h>
/*
 * Initialise reg reconciliation run from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_reg_reconciliation_run_init(UmiReconciliationRun *run,const char *run_id){ UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(run==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(run,0,sizeof *run); s=umi_reg_copy_text(run->run_id,sizeof run->run_id,run_id); return s; }
/*
 * Provide the reg reconciliation run record operation used by this module and its client
 * applications.
 */
UmiStatus umi_reg_reconciliation_run_record(UmiReconciliationRun *run,double left,double right,double tolerance){ double d; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(run==NULL||!umi_reg_number_valid(left)||!umi_reg_number_valid(right)||!umi_reg_number_valid(tolerance)||tolerance<0.0)return UMI_STATUS_INVALID_ARGUMENT; d=fabs(left-right);run->absolute_difference+=d;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(d<=tolerance)++run->matched;/* Use this fallback path when the earlier condition does not apply. */ else ++run->broken;return UMI_STATUS_OK; }
