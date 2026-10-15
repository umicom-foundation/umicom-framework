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
#include "umicom/base/value_archive.h"
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

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_commodity_delivery_window_archive_encode(const UmiCommodityDeliveryWindow *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_commodity_delivery_window_archive_decode(const void *bytes, size_t byte_count,
    UmiCommodityDeliveryWindow *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_commodity_delivery_window_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiCommodityDeliveryWindow *expected, UmiCommodityDeliveryWindow *value);


#ifdef __cplusplus
}
#endif

#endif
