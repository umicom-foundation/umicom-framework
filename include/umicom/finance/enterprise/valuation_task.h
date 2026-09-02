/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/valuation_task.h
 *
 * PURPOSE:
 *   Represent one instrument-level valuation task within a job.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_TASK_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_TASK_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the enterprise valuation task data shared with callers of this public
 * contract.
 */
typedef struct UmiEnterpriseValuationTask { char task_id[UMI_ENTERPRISE_ID_CAPACITY]; char job_id[UMI_ENTERPRISE_ID_CAPACITY]; UmiFinancialId product_id; double quantity; UmiEnterpriseWorkState state; } UmiEnterpriseValuationTask;
/* Initialise a pending finite-quantity valuation task. */
UmiStatus umi_enterprise_valuation_task_init(UmiEnterpriseValuationTask *task,const char *task_id,const char *job_id,const char *product_id,double quantity);

#ifdef __cplusplus
}
#endif

#endif
