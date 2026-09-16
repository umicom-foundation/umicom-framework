/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/execution_store.c
 *
 * PURPOSE:
 *   Maintain a bounded in-memory execution history for deterministic tests and replay.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of execution store. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/execution_store.h"
#include "umicom/trading/execution_report.h"
#include "umicom/finance/identifier.h"

/*
 * Initialise execution store from caller-provided values so later operations receive a
 * known state.
 */
void umi_execution_store_init(UmiExecutionStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store != NULL) {
        store->count = 0U;
    }
}

/* Add execution store only after its inputs and available capacity have been checked. */
UmiStatus umi_execution_store_add(UmiExecutionStore *store,
                                  const UmiExecutionReport *report)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || !umi_execution_report_valid(report)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    const UmiExecutionReport *existing = NULL;
    const UmiStatus found = UmiExecutionStoreFind(store, &report->execution_id, &existing);
    if (found == UMI_STATUS_OK) return UMI_STATUS_ALREADY_EXISTS;
    if (found != UMI_STATUS_NOT_FOUND) return found;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->count >= UMI_TRADING_MAX_ORDERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    store->reports[store->count] = *report;
    store->count++;
    return UMI_STATUS_OK;
}

/* A const borrowed view into the retained store, valid until the owner mutates
 * or destroys it. The count is checked before scanning fixed-size storage. */
UmiStatus UmiExecutionStoreFind(const UmiExecutionStore *store,
    const UmiFinancialId *executionId, const UmiExecutionReport **outReport)
{
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReport = NULL;
    if (store == NULL || !umi_financial_id_valid(executionId))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (store->count > UMI_TRADING_MAX_ORDERS) return UMI_STATUS_INVALID_STATE;
    for (size_t index = 0U; index < store->count; ++index) {
        if (!umi_execution_report_valid(&store->reports[index]))
            return UMI_STATUS_INVALID_STATE;
        if (umi_financial_id_equal(&store->reports[index].execution_id, executionId)) {
            *outReport = &store->reports[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
