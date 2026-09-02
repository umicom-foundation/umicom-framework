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

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->count >= UMI_TRADING_MAX_ORDERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    store->reports[store->count] = *report;
    store->count++;
    return UMI_STATUS_OK;
}
