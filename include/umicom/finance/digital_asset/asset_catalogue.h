/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/asset_catalogue.h
 *
 * PURPOSE:
 *   Define a bounded catalogue of digital assets.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_ASSET_CATALOGUE_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_ASSET_CATALOGUE_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"
#include "umicom/finance/digital_asset/asset_descriptor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDigitalAssetCatalogue {
    UmiDigitalAssetDescriptor items[UMI_DIGITAL_ASSET_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiDigitalAssetCatalogue;

/* Reset a bounded catalogue before loading reference data. */
void umi_digital_asset_asset_catalogue_init(UmiDigitalAssetCatalogue *catalogue);

/* Add a unique item and advance the deterministic catalogue revision. */
UmiStatus umi_digital_asset_asset_catalogue_add(UmiDigitalAssetCatalogue *catalogue, const UmiDigitalAssetDescriptor *item);

/* Resolve an item by its stable Framework identifier. */
const UmiDigitalAssetDescriptor *umi_digital_asset_asset_catalogue_find(const UmiDigitalAssetCatalogue *catalogue, const char *id);

#ifdef __cplusplus
}
#endif

#endif
