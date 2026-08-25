/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/unit_catalogue.h
 *
 * PURPOSE:
 *   Define a bounded catalogue of physical commodity units.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_UNIT_CATALOGUE_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_UNIT_CATALOGUE_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"
#include "umicom/finance/commodity/unit_of_measure.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommodityUnitCatalogue {
    UmiCommodityUnitOfMeasure items[UMI_COMMODITY_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiCommodityUnitCatalogue;

/* Reset a bounded catalogue before loading reference data. */
void umi_commodity_unit_catalogue_init(UmiCommodityUnitCatalogue *catalogue);

/* Add a unique item and advance the deterministic catalogue revision. */
UmiStatus umi_commodity_unit_catalogue_add(UmiCommodityUnitCatalogue *catalogue, const UmiCommodityUnitOfMeasure *item);

/* Resolve an item by its stable Framework identifier. */
const UmiCommodityUnitOfMeasure *umi_commodity_unit_catalogue_find(const UmiCommodityUnitCatalogue *catalogue, const char *id);

#ifdef __cplusplus
}
#endif

#endif
