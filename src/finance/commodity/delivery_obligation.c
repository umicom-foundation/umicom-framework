/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/delivery_obligation.c
 *
 * PURPOSE:
 *   Implement a contract delivery obligation with quantity, due time and lifecycle state.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/commodity/delivery_obligation.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_delivery_obligation_init(UmiCommodityDeliveryObligation *value, const char *id, const char *contract_id, int64_t units, int32_t scale, const char *unit_code, int64_t due_time_ms)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || units <= 0 || scale < 0 || due_time_ms < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->id.value, sizeof value->id.value, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->contract_id.value, sizeof value->contract_id.value, contract_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->quantity.unit_code, sizeof value->quantity.unit_code, unit_code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->quantity.units = units;
    value->quantity.scale = scale;
    value->due_time_ms = due_time_ms;
    value->state = UMI_COMMODITY_DELIVERY_PLANNED;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_delivery_obligation_valid(const UmiCommodityDeliveryObligation *value)
{
    return value != NULL && (umi_commodity_text_valid(value->id.value) && umi_commodity_text_valid(value->contract_id.value) && value->quantity.units > 0 && value->due_time_ms >= 0);
}
