/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/asset_catalogue.c
 *
 * PURPOSE:
 *   Implement a bounded catalogue of digital assets.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/digital_asset/asset_catalogue.h"

#include <string.h>

/* Reset catalogue state without heap allocation. */
void umi_digital_asset_asset_catalogue_init(UmiDigitalAssetCatalogue *catalogue)
{
    if (catalogue != NULL) {
        memset(catalogue, 0, sizeof *catalogue);
    }
}

/* Reject duplicates and preserve insertion order for reproducible snapshots. */
UmiStatus umi_digital_asset_asset_catalogue_add(UmiDigitalAssetCatalogue *catalogue, const UmiDigitalAssetDescriptor *item)
{
    size_t index;
    if (catalogue == NULL || item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].id.value, (*item).id.value) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (catalogue->count >= UMI_DIGITAL_ASSET_MAX_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    catalogue->items[catalogue->count++] = *item;
    catalogue->revision++;
    return UMI_STATUS_OK;
}

/* Resolve reference data without exposing catalogue storage mutability. */
const UmiDigitalAssetDescriptor *umi_digital_asset_asset_catalogue_find(const UmiDigitalAssetCatalogue *catalogue, const char *id)
{
    size_t index;
    if (catalogue == NULL || id == NULL) {
        return NULL;
    }
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].id.value, id) == 0) {
            return &catalogue->items[index];
        }
    }
    return NULL;
}
