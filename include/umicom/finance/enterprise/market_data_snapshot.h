/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/market_data_snapshot.h
 *
 * PURPOSE:
 *   Capture a coherent bounded market-data cut for valuation.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_SNAPSHOT_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_SNAPSHOT_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/market_data_update.h"

typedef struct UmiEnterpriseMarketDataSnapshot {
    char snapshot_id[UMI_ENTERPRISE_ID_CAPACITY];
    int64_t as_of_ms;
    UmiEnterpriseMarketDataUpdate updates[UMI_ENTERPRISE_MAX_ITEMS];
    size_t count;
} UmiEnterpriseMarketDataSnapshot;

/* Initialise a coherent market-data snapshot at an explicit valuation time. */
UmiStatus umi_enterprise_market_data_snapshot_init(UmiEnterpriseMarketDataSnapshot *snapshot, const char *snapshot_id, int64_t as_of_ms);
/* Add an update whose event time does not exceed the snapshot cut. */
UmiStatus umi_enterprise_market_data_snapshot_add(UmiEnterpriseMarketDataSnapshot *snapshot, const UmiEnterpriseMarketDataUpdate *update);

#ifdef __cplusplus
}
#endif

#endif
