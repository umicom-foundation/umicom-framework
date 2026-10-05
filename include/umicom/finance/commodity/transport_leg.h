/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/transport_leg.h
 *
 * PURPOSE:
 *   Define one ordered physical leg within a commodity transport route.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_TRANSPORT_LEG_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_TRANSPORT_LEG_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the commodity transport leg data shared with callers of this public contract.
 */
typedef struct UmiCommodityTransportLeg {
    UmiCommodityId route_id;
    uint32_t sequence;
    UmiCommodityId origin_location_id;
    UmiCommodityId destination_location_id;
    int64_t planned_departure_ms;
    int64_t planned_arrival_ms;
} UmiCommodityTransportLeg;

/* Initialise a bounded transport leg record for reusable Framework workflows. */
UmiStatus umi_commodity_transport_leg_init(UmiCommodityTransportLeg *value, const char *route_id, uint32_t sequence, const char *origin_location_id, const char *destination_location_id, int64_t planned_departure_ms, int64_t planned_arrival_ms);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_transport_leg_valid(const UmiCommodityTransportLeg *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_commodity_transport_leg_archive_encode(const UmiCommodityTransportLeg *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_commodity_transport_leg_archive_decode(const void *bytes, size_t byte_count,
    UmiCommodityTransportLeg *value);

#ifdef __cplusplus
}
#endif

#endif
