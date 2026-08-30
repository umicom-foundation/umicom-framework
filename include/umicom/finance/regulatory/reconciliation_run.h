/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/reconciliation_run.h
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_RECONCILIATION_RUN_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_RECONCILIATION_RUN_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReconciliationRun { char run_id[UMI_REG_ID_CAPACITY]; size_t matched; size_t broken; double absolute_difference; } UmiReconciliationRun;
/* Initialise an empty reconciliation run. */
UmiStatus umi_reg_reconciliation_run_init(UmiReconciliationRun *run,const char *run_id);
/* Record one pair and classify it using an absolute tolerance. */
UmiStatus umi_reg_reconciliation_run_record(UmiReconciliationRun *run,double left,double right,double tolerance);

#ifdef __cplusplus
}
#endif

#endif
