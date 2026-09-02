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

#ifdef __cplusplus
}
#endif

#endif
