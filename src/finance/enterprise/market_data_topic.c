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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/market_data_topic.h"

#include <string.h>

UmiStatus umi_enterprise_market_data_topic_init(UmiEnterpriseMarketDataTopic *topic, const char *topic_id, const char *instrument_id, const char *field)
{
    UmiStatus status;
    if (topic == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(topic, 0, sizeof *topic);
    status = umi_quant_copy_text(topic->topic_id, sizeof topic->topic_id, topic_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_quant_copy_text(topic->instrument_id.value, sizeof topic->instrument_id.value, instrument_id);
    if (status != UMI_STATUS_OK) return status;
    return umi_quant_copy_text(topic->field, sizeof topic->field, field);
}
