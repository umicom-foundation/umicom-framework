/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/market_data_snapshot_store.c
 *
 * PURPOSE:
 *   Store immutable market-data cuts in monotonic as-of order.
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

#include "umicom/finance/enterprise/market_data_snapshot_store.h"

#include <string.h>

/*
 * Initialise enterprise market data snapshot store from caller-provided values so later
 * operations receive a known state.
 */
void umi_enterprise_market_data_snapshot_store_init(UmiEnterpriseMarketDataSnapshotStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store != NULL) memset(store, 0, sizeof *store);
}
/*
 * Add enterprise market data snapshot store only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_enterprise_market_data_snapshot_store_append(UmiEnterpriseMarketDataSnapshotStore *store, const UmiEnterpriseMarketDataSnapshot *snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->count >= 16U) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->count > 0U && snapshot->as_of_ms <= store->snapshots[store->count-1U].as_of_ms) return UMI_STATUS_INVALID_STATE;
    store->snapshots[store->count++] = *snapshot; return UMI_STATUS_OK;
}
/*
 * Provide the enterprise market data snapshot store latest operation used by this module
 * and its client applications.
 */
const UmiEnterpriseMarketDataSnapshot *umi_enterprise_market_data_snapshot_store_latest(const UmiEnterpriseMarketDataSnapshotStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || store->count == 0U) return NULL;
    return &store->snapshots[store->count-1U];
}
