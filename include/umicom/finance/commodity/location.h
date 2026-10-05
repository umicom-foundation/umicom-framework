/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/location.h
 *
 * PURPOSE:
 *   Define a physical delivery, storage or logistics location.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_LOCATION_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_LOCATION_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the commodity location data shared with callers of this public contract.
 */
typedef struct UmiCommodityLocation {
    UmiCommodityId id;
    char name[UMI_COMMODITY_NAME_CAPACITY];
    char country_code[4];
    bool active;
} UmiCommodityLocation;

/* Initialise a bounded location record for reusable Framework workflows. */
UmiStatus umi_commodity_location_init(UmiCommodityLocation *value, const char *id, const char *name, const char *country_code);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_location_valid(const UmiCommodityLocation *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_commodity_location_archive_encode(const UmiCommodityLocation *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_commodity_location_archive_decode(const void *bytes, size_t byte_count,
    UmiCommodityLocation *value);

#ifdef __cplusplus
}
#endif

#endif
