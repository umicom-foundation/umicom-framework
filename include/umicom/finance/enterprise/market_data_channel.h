/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/market_data_channel.h
 *
 * PURPOSE:
 *   Track ordered sequence and heartbeat evidence for a market-data channel.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_CHANNEL_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_MARKET_DATA_CHANNEL_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the enterprise market data channel data shared with callers of this public
 * contract.
 */
typedef struct UmiEnterpriseMarketDataChannel {
    char channel_id[UMI_ENTERPRISE_ID_CAPACITY];
    uint64_t last_sequence;
    int64_t last_event_ms;
    int connected;
} UmiEnterpriseMarketDataChannel;

/* Initialise a disconnected channel before transport attachment. */
UmiStatus umi_enterprise_market_data_channel_init(UmiEnterpriseMarketDataChannel *channel, const char *channel_id);
/* Accept only strictly increasing sequence numbers and non-decreasing timestamps. */
UmiStatus umi_enterprise_market_data_channel_accept(UmiEnterpriseMarketDataChannel *channel, uint64_t sequence, int64_t event_ms);

#ifdef __cplusplus
}
#endif

#endif
