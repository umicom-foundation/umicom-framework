/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/report_instance_store.c
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

#include "umicom/finance/regulatory/report_instance_store.h"

#include <string.h>
void umi_reg_report_instance_store_init(UmiReportInstanceStore *store) { if (store != NULL) memset(store, 0, sizeof *store); }
const UmiReportInstance *umi_reg_report_instance_store_find(const UmiReportInstanceStore *store, const char *report_id)
{
    size_t i;
    if (store == NULL || report_id == NULL) return NULL;
    for (i=0U;i<store->count;++i) if (strcmp(store->items[i].report_id, report_id)==0) return &store->items[i];
    return NULL;
}
UmiStatus umi_reg_report_instance_store_add(UmiReportInstanceStore *store, const UmiReportInstance *report)
{
    if (store == NULL || report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (store->count >= UMI_REG_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_reg_report_instance_store_find(store, report->report_id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    store->items[store->count++] = *report;
    return UMI_STATUS_OK;
}
