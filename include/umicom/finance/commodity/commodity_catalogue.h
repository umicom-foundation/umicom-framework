/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/commodity_catalogue.h
 *
 * PURPOSE:
 *   Define a bounded deterministic catalogue of reusable commodity reference data.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_COMMODITY_CATALOGUE_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_COMMODITY_CATALOGUE_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"
#include "umicom/finance/commodity/commodity_descriptor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommodityCatalogue {
    UmiCommodityDescriptor items[UMI_COMMODITY_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiCommodityCatalogue;

/* Reset a bounded catalogue before loading reference data. */
void umi_commodity_commodity_catalogue_init(UmiCommodityCatalogue *catalogue);

/* Add a unique item and advance the deterministic catalogue revision. */
UmiStatus umi_commodity_commodity_catalogue_add(UmiCommodityCatalogue *catalogue, const UmiCommodityDescriptor *item);

/* Resolve an item by its stable Framework identifier. */
const UmiCommodityDescriptor *umi_commodity_commodity_catalogue_find(const UmiCommodityCatalogue *catalogue, const char *id);

#ifdef __cplusplus
}
#endif

#endif
