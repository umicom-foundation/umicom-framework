/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/storage_facility.h
 *
 * PURPOSE:
 *   Define storage capacity at a physical commodity location.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_STORAGE_FACILITY_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_STORAGE_FACILITY_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the commodity storage facility data shared with callers of this public
 * contract.
 */
typedef struct UmiCommodityStorageFacility {
    UmiCommodityId id;
    UmiCommodityId location_id;
    UmiCommodityQuantity capacity;
    bool active;
} UmiCommodityStorageFacility;

/* Initialise a bounded storage facility record for reusable Framework workflows. */
UmiStatus umi_commodity_storage_facility_init(UmiCommodityStorageFacility *value, const char *id, const char *location_id, int64_t capacity_units, int32_t scale, const char *unit_code);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_commodity_storage_facility_valid(const UmiCommodityStorageFacility *value);

#ifdef __cplusplus
}
#endif

#endif
