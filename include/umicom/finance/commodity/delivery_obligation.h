/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/delivery_obligation.h
 *
 * PURPOSE:
 *   Define a contract delivery obligation with quantity, due time and lifecycle state.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_DELIVERY_OBLIGATION_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_DELIVERY_OBLIGATION_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the commodity delivery obligation data shared with callers of this public
 * contract.
 */
typedef struct UmiCommodityDeliveryObligation {
    UmiCommodityId id;
    UmiCommodityId contract_id;
    UmiCommodityQuantity quantity;
    int64_t due_time_ms;
    UmiCommodityDeliveryState state;
} UmiCommodityDeliveryObligation;

/* Initialise a bounded delivery obligation record for reusable Framework workflows. */
UmiStatus umi_commodity_delivery_obligation_init(UmiCommodityDeliveryObligation *value, const char *id, const char *contract_id, int64_t units, int32_t scale, const char *unit_code, int64_t due_time_ms);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_delivery_obligation_valid(const UmiCommodityDeliveryObligation *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_commodity_delivery_obligation_archive_encode(const UmiCommodityDeliveryObligation *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_commodity_delivery_obligation_archive_decode(const void *bytes, size_t byte_count,
    UmiCommodityDeliveryObligation *value);

#ifdef __cplusplus
}
#endif

#endif
