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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/regulatory/reconciliation_run.h"

#include <math.h>
#include <string.h>
UmiStatus umi_reg_reconciliation_run_init(UmiReconciliationRun *run,const char *run_id){ UmiStatus s; if(run==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(run,0,sizeof *run); s=umi_reg_copy_text(run->run_id,sizeof run->run_id,run_id); return s; }
UmiStatus umi_reg_reconciliation_run_record(UmiReconciliationRun *run,double left,double right,double tolerance){ double d; if(run==NULL||!umi_reg_number_valid(left)||!umi_reg_number_valid(right)||!umi_reg_number_valid(tolerance)||tolerance<0.0)return UMI_STATUS_INVALID_ARGUMENT; d=fabs(left-right);run->absolute_difference+=d;if(d<=tolerance)++run->matched;else ++run->broken;return UMI_STATUS_OK; }
