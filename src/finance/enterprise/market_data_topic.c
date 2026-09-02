/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/market_data_topic.c
 *
 * PURPOSE:
 *   Define canonical market-data topics independently from vendor feed identifiers.
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

#include "umicom/finance/enterprise/market_data_topic.h"

#include <string.h>

/*
 * Initialise enterprise market data topic from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_enterprise_market_data_topic_init(UmiEnterpriseMarketDataTopic *topic, const char *topic_id, const char *instrument_id, const char *field)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (topic == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(topic, 0, sizeof *topic);
    status = umi_quant_copy_text(topic->topic_id, sizeof topic->topic_id, topic_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_quant_copy_text(topic->instrument_id.value, sizeof topic->instrument_id.value, instrument_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_quant_copy_text(topic->field, sizeof topic->field, field);
}
