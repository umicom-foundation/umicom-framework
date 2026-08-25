/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/reconciliation_item.h
 *
 * PURPOSE:
 *   Represent one pair of financial values selected for reconciliation.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_RECONCILIATION_ITEM_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_RECONCILIATION_ITEM_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReconciliationItem {
    char item_id[UMI_REG_ID_CAPACITY];
    double left_value;
    double right_value;
    double tolerance;
} UmiReconciliationItem;

/* Initialise one validated represent one pair of financial values selected for reconciliation. record. */
UmiStatus umi_reg_reconciliation_item_init(UmiReconciliationItem *record, const char *item_id, double left_value, double right_value, double tolerance);

#ifdef __cplusplus
}
#endif

#endif
