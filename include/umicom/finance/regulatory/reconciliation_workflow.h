/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/reconciliation_workflow.h
 *
 * PURPOSE:
 *   Track ownership and remediation state for reconciliation breaks.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_RECONCILIATION_WORKFLOW_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_RECONCILIATION_WORKFLOW_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReconciliationWorkflow {
    char workflow_id[UMI_REG_ID_CAPACITY];
    char break_id[UMI_REG_ID_CAPACITY];
    char owner_id[UMI_REG_ID_CAPACITY];
    int resolved;
} UmiReconciliationWorkflow;

/* Initialise one validated track ownership and remediation state for reconciliation breaks. record. */
UmiStatus umi_reg_reconciliation_workflow_init(UmiReconciliationWorkflow *record, const char *workflow_id, const char *break_id, const char *owner_id, int resolved);

#ifdef __cplusplus
}
#endif

#endif
