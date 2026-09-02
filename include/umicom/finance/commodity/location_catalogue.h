/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/location_catalogue.h
 *
 * PURPOSE:
 *   Define a bounded catalogue of physical commodity locations.
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_LOCATION_CATALOGUE_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_LOCATION_CATALOGUE_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"
#include "umicom/finance/commodity/location.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the commodity location catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiCommodityLocationCatalogue {
    UmiCommodityLocation items[UMI_COMMODITY_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiCommodityLocationCatalogue;

/* Reset a bounded catalogue before loading reference data. */
void umi_commodity_location_catalogue_init(UmiCommodityLocationCatalogue *catalogue);

/* Add a unique item and advance the deterministic catalogue revision. */
UmiStatus umi_commodity_location_catalogue_add(UmiCommodityLocationCatalogue *catalogue, const UmiCommodityLocation *item);

/* Resolve an item by its stable Framework identifier. */
const UmiCommodityLocation *umi_commodity_location_catalogue_find(const UmiCommodityLocationCatalogue *catalogue, const char *id);

#ifdef __cplusplus
}
#endif

#endif
