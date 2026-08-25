/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/market_data_snapshot_store.h
 *
 * PURPOSE:
 *   Store immutable market-data cuts in monotonic as-of order.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_SNAPSHOT_STORE_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_SNAPSHOT_STORE_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/market_data_snapshot.h"

typedef struct UmiEnterpriseMarketDataSnapshotStore {
    UmiEnterpriseMarketDataSnapshot snapshots[16U];
    size_t count;
} UmiEnterpriseMarketDataSnapshotStore;

/* Initialise an empty bounded snapshot history. */
void umi_enterprise_market_data_snapshot_store_init(UmiEnterpriseMarketDataSnapshotStore *store);
/* Append a snapshot only when its as-of time advances. */
UmiStatus umi_enterprise_market_data_snapshot_store_append(UmiEnterpriseMarketDataSnapshotStore *store, const UmiEnterpriseMarketDataSnapshot *snapshot);
/* Return the most recent stored snapshot or NULL when empty. */
const UmiEnterpriseMarketDataSnapshot *umi_enterprise_market_data_snapshot_store_latest(const UmiEnterpriseMarketDataSnapshotStore *store);

#ifdef __cplusplus
}
#endif

#endif
