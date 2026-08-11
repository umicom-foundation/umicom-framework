/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/execution_store.c
 *
 * PURPOSE:
 *   Maintain a bounded in-memory execution history for deterministic tests and replay.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of execution store. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/execution_store.h"
#include "umicom/trading/execution_report.h"

void umi_execution_store_init(UmiExecutionStore *store)
{
    if (store != NULL) {
        store->count = 0U;
    }
}

UmiStatus umi_execution_store_add(UmiExecutionStore *store,
                                  const UmiExecutionReport *report)
{
    if (store == NULL || !umi_execution_report_valid(report)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (store->count >= UMI_TRADING_MAX_ORDERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    store->reports[store->count] = *report;
    store->count++;
    return UMI_STATUS_OK;
}
