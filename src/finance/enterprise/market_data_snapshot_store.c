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

void umi_enterprise_market_data_snapshot_store_init(UmiEnterpriseMarketDataSnapshotStore *store)
{
    if (store != NULL) memset(store, 0, sizeof *store);
}
UmiStatus umi_enterprise_market_data_snapshot_store_append(UmiEnterpriseMarketDataSnapshotStore *store, const UmiEnterpriseMarketDataSnapshot *snapshot)
{
    if (store == NULL || snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (store->count >= 16U) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (store->count > 0U && snapshot->as_of_ms <= store->snapshots[store->count-1U].as_of_ms) return UMI_STATUS_INVALID_STATE;
    store->snapshots[store->count++] = *snapshot; return UMI_STATUS_OK;
}
const UmiEnterpriseMarketDataSnapshot *umi_enterprise_market_data_snapshot_store_latest(const UmiEnterpriseMarketDataSnapshotStore *store)
{
    if (store == NULL || store->count == 0U) return NULL;
    return &store->snapshots[store->count-1U];
}
