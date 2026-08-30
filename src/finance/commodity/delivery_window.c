/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/delivery_window.c
 *
 * PURPOSE:
 *   Implement the permitted start and end time for a commodity delivery.
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

#include "umicom/finance/commodity/delivery_window.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_delivery_window_init(UmiCommodityDeliveryWindow *value, int64_t start_time_ms, int64_t end_time_ms, bool inclusive_end)
{
    if (value == NULL || start_time_ms < 0 || end_time_ms <= start_time_ms) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    value->start_time_ms = start_time_ms;
    value->end_time_ms = end_time_ms;
    value->inclusive_end = inclusive_end;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_delivery_window_valid(const UmiCommodityDeliveryWindow *value)
{
    return value != NULL && (value->start_time_ms >= 0 && value->end_time_ms > value->start_time_ms);
}
