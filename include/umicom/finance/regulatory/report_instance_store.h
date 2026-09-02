/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/report_instance_store.h
 *
 * PURPOSE:
 *   Store bounded regulatory report instances with duplicate protection.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REPORT_INSTANCE_STORE_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REPORT_INSTANCE_STORE_H

#include "umicom/finance/regulatory/types.h"
#include "umicom/finance/regulatory/report_instance.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/regulatory/report_instance.h"
/**
 * Represent the report instance store data shared with callers of this public contract.
 */
typedef struct UmiReportInstanceStore {
    UmiReportInstance items[UMI_REG_MAX_ITEMS];
    size_t count;
} UmiReportInstanceStore;
/* Initialise an empty report-instance store. */
void umi_reg_report_instance_store_init(UmiReportInstanceStore *store);
/* Add one unique report instance to the store. */
UmiStatus umi_reg_report_instance_store_add(UmiReportInstanceStore *store, const UmiReportInstance *report);
/* Find a report instance by stable report identifier. */
const UmiReportInstance *umi_reg_report_instance_store_find(const UmiReportInstanceStore *store, const char *report_id);

#ifdef __cplusplus
}
#endif

#endif
