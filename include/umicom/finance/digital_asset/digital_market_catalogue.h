/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/digital_market_catalogue.h
 *
 * PURPOSE:
 *   Define a bounded catalogue of digital-asset markets for exchange composition.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_DIGITAL_MARKET_CATALOGUE_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_DIGITAL_MARKET_CATALOGUE_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"
#include "umicom/finance/digital_asset/digital_market.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDigitalMarketCatalogue {
    UmiDigitalMarket items[UMI_DIGITAL_ASSET_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiDigitalMarketCatalogue;

/* Reset a bounded catalogue before loading reference data. */
void umi_digital_asset_digital_market_catalogue_init(UmiDigitalMarketCatalogue *catalogue);

/* Add a unique item and advance the deterministic catalogue revision. */
UmiStatus umi_digital_asset_digital_market_catalogue_add(UmiDigitalMarketCatalogue *catalogue, const UmiDigitalMarket *item);

/* Resolve an item by its stable Framework identifier. */
const UmiDigitalMarket *umi_digital_asset_digital_market_catalogue_find(const UmiDigitalMarketCatalogue *catalogue, const char *id);

#ifdef __cplusplus
}
#endif

#endif
