/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/market_data_update.c
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

#include "umicom/finance/enterprise/market_data_update.h"

#include <string.h>

UmiStatus umi_enterprise_market_data_update_init(UmiEnterpriseMarketDataUpdate *update, const char *topic_id, double value, uint64_t sequence, int64_t event_time_ms)
{
    UmiStatus status;
    if (update == NULL || !umi_quant_number_valid(value) || sequence == 0U || event_time_ms < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(update, 0, sizeof *update);
    status = umi_quant_copy_text(update->topic_id, sizeof update->topic_id, topic_id);
    if (status != UMI_STATUS_OK) return status;
    update->value = value;
    update->sequence = sequence;
    update->event_time_ms = event_time_ms;
    return UMI_STATUS_OK;
}
