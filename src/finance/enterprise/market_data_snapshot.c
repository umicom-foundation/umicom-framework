/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/market_data_snapshot.c
 *
 * PURPOSE:
 *   Capture a coherent bounded market-data cut for valuation.
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

#include "umicom/finance/enterprise/market_data_snapshot.h"

#include <string.h>

/*
 * Initialise enterprise market data snapshot from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_enterprise_market_data_snapshot_init(UmiEnterpriseMarketDataSnapshot *snapshot, const char *snapshot_id, int64_t as_of_ms)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (snapshot == NULL || as_of_ms < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(snapshot, 0, sizeof *snapshot);
    status = umi_quant_copy_text(snapshot->snapshot_id, sizeof snapshot->snapshot_id, snapshot_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    snapshot->as_of_ms = as_of_ms;
    return UMI_STATUS_OK;
}

/*
 * Add enterprise market data snapshot only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_enterprise_market_data_snapshot_add(UmiEnterpriseMarketDataSnapshot *snapshot, const UmiEnterpriseMarketDataUpdate *update)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (snapshot == NULL || update == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (update->event_time_ms > snapshot->as_of_ms) return UMI_STATUS_INVALID_STATE;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (snapshot->count >= UMI_ENTERPRISE_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    snapshot->updates[snapshot->count++] = *update;
    return UMI_STATUS_OK;
}
