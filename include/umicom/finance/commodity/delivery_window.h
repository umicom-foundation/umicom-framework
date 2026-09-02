/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/delivery_window.h
 *
 * PURPOSE:
 *   Define the permitted start and end time for a commodity delivery.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_DELIVERY_WINDOW_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_DELIVERY_WINDOW_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the commodity delivery window data shared with callers of this public
 * contract.
 */
typedef struct UmiCommodityDeliveryWindow {
    int64_t start_time_ms;
    int64_t end_time_ms;
    bool inclusive_end;
} UmiCommodityDeliveryWindow;

/* Initialise a bounded delivery window record for reusable Framework workflows. */
UmiStatus umi_commodity_delivery_window_init(UmiCommodityDeliveryWindow *value, int64_t start_time_ms, int64_t end_time_ms, bool inclusive_end);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_delivery_window_valid(const UmiCommodityDeliveryWindow *value);

#ifdef __cplusplus
}
#endif

#endif
