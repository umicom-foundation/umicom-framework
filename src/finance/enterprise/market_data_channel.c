/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/market_data_channel.c
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

#include "umicom/finance/enterprise/market_data_channel.h"

#include <string.h>

/*
 * Initialise enterprise market data channel from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_enterprise_market_data_channel_init(UmiEnterpriseMarketDataChannel *channel, const char *channel_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (channel == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(channel, 0, sizeof *channel);
    status = umi_quant_copy_text(channel->channel_id, sizeof channel->channel_id, channel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) channel->connected = 1;
    return status;
}

/*
 * Provide the enterprise market data channel accept operation used by this module and its
 * client applications.
 */
UmiStatus umi_enterprise_market_data_channel_accept(UmiEnterpriseMarketDataChannel *channel, uint64_t sequence, int64_t event_ms)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (channel == NULL || channel->connected == 0 || sequence == 0U || event_ms < 0) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (channel->last_sequence != 0U && sequence <= channel->last_sequence) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (channel->last_event_ms != 0 && event_ms < channel->last_event_ms) return UMI_STATUS_INVALID_STATE;
    channel->last_sequence = sequence;
    channel->last_event_ms = event_ms;
    return UMI_STATUS_OK;
}
