/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/reconciliation_break.h
 *
 * PURPOSE:
 *   Capture a material unresolved break produced by reconciliation.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_RECONCILIATION_BREAK_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_RECONCILIATION_BREAK_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReconciliationBreak {
    char break_id[UMI_REG_ID_CAPACITY];
    char item_id[UMI_REG_ID_CAPACITY];
    double difference;
    UmiRegSeverity severity;
} UmiReconciliationBreak;

/* Initialise one validated capture a material unresolved break produced by reconciliation. record. */
UmiStatus umi_reg_reconciliation_break_init(UmiReconciliationBreak *record, const char *break_id, const char *item_id, double difference, UmiRegSeverity severity);

#ifdef __cplusplus
}
#endif

#endif
