/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/nomination.c
 *
 * PURPOSE:
 *   Implement a quantity nomination against a physical contract and delivery window.
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

#include "umicom/finance/commodity/nomination.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_nomination_init(UmiCommodityNomination *value, const char *id, const char *contract_id, int64_t units, int32_t scale, const char *unit_code, int64_t window_start_ms, int64_t window_end_ms)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || units <= 0 || scale < 0 || window_start_ms < 0 || window_end_ms <= window_start_ms) return UMI_STATUS_INVALID_ARGUMENT;
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
    value->window_start_ms = window_start_ms;
    value->window_end_ms = window_end_ms;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_nomination_valid(const UmiCommodityNomination *value)
{
    return value != NULL && (umi_commodity_text_valid(value->id.value) && umi_commodity_text_valid(value->contract_id.value) && value->quantity.units > 0 && value->window_end_ms > value->window_start_ms);
}
