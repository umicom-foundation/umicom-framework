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
/*
 * Initialise reg report instance store from caller-provided values so later operations
 * receive a known state.
 */
void umi_reg_report_instance_store_init(UmiReportInstanceStore *store) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (store != NULL) memset(store, 0, sizeof *store); }
/*
 * Find reg report instance store while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiReportInstance *umi_reg_report_instance_store_find(const UmiReportInstanceStore *store, const char *report_id)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || report_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i=0U;i<store->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(store->items[i].report_id, report_id)==0) return &store->items[i];
    return NULL;
}
/*
 * Add reg report instance store only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_reg_report_instance_store_add(UmiReportInstanceStore *store, const UmiReportInstance *report)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->count >= UMI_REG_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_reg_report_instance_store_find(store, report->report_id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    store->items[store->count++] = *report;
    return UMI_STATUS_OK;
}
