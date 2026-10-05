/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/inventory_lot.h
 *
 * PURPOSE:
 *   Define an auditable physical inventory lot and its available quantity.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_INVENTORY_LOT_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_INVENTORY_LOT_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the commodity inventory lot data shared with callers of this public contract.
 */
typedef struct UmiCommodityInventoryLot {
    UmiCommodityId id;
    UmiCommodityId commodity_id;
    UmiCommodityId facility_id;
    UmiCommodityQuantity quantity;
    int64_t received_time_ms;
    bool quality_accepted;
} UmiCommodityInventoryLot;

/* Initialise a bounded inventory lot record for reusable Framework workflows. */
UmiStatus umi_commodity_inventory_lot_init(UmiCommodityInventoryLot *value, const char *id, const char *commodity_id, const char *facility_id, int64_t units, int32_t scale, const char *unit_code, int64_t received_time_ms);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_inventory_lot_valid(const UmiCommodityInventoryLot *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_commodity_inventory_lot_archive_encode(const UmiCommodityInventoryLot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_commodity_inventory_lot_archive_decode(const void *bytes, size_t byte_count,
    UmiCommodityInventoryLot *value);

#ifdef __cplusplus
}
#endif

#endif
