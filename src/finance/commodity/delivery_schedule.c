/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/delivery_schedule.c
 *
 * PURPOSE:
 *   Implement an ordered bounded schedule of physical delivery obligations.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/commodity/delivery_schedule.h"

#include <string.h>

/* Initialise bounded schedule storage for deterministic delivery planning. */
UmiStatus umi_commodity_delivery_schedule_init(UmiCommodityDeliverySchedule *value, const char *contract_id)
{
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    return umi_commodity_copy_text(value->contract_id.value, sizeof value->contract_id.value, contract_id);
}

/* Keep obligations time ordered so downstream planning can iterate without sorting. */
UmiStatus umi_commodity_delivery_schedule_add(UmiCommodityDeliverySchedule *value, const UmiCommodityDeliveryObligation *obligation)
{
    if (value == NULL || obligation == NULL || !umi_commodity_delivery_obligation_valid(obligation)) return UMI_STATUS_INVALID_ARGUMENT;
    if (value->count >= UMI_COMMODITY_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (value->count > 0U && obligation->due_time_ms < value->obligations[value->count - 1U].due_time_ms) return UMI_STATUS_INVALID_STATE;
    value->obligations[value->count++] = *obligation;
    return UMI_STATUS_OK;
}
