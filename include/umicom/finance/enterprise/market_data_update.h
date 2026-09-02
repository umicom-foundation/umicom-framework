/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/market_data_update.h
 *
 * PURPOSE:
 *   Represent one ordered finite market-data observation.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_UPDATE_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_UPDATE_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the enterprise market data update data shared with callers of this public
 * contract.
 */
typedef struct UmiEnterpriseMarketDataUpdate {
    char topic_id[UMI_ENTERPRISE_ID_CAPACITY];
    double value;
    uint64_t sequence;
    int64_t event_time_ms;
} UmiEnterpriseMarketDataUpdate;

/* Initialise one feed-normalised market-data update. */
UmiStatus umi_enterprise_market_data_update_init(UmiEnterpriseMarketDataUpdate *update, const char *topic_id, double value, uint64_t sequence, int64_t event_time_ms);

#ifdef __cplusplus
}
#endif

#endif
