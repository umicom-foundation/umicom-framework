/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/network_catalogue.h
 *
 * PURPOSE:
 *   Define a bounded catalogue of digital-asset network descriptors.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_NETWORK_CATALOGUE_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_NETWORK_CATALOGUE_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"
#include "umicom/finance/digital_asset/network_descriptor.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDigitalNetworkCatalogue {
    UmiDigitalNetworkDescriptor items[UMI_DIGITAL_ASSET_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiDigitalNetworkCatalogue;

/* Reset a bounded catalogue before loading reference data. */
void umi_digital_asset_network_catalogue_init(UmiDigitalNetworkCatalogue *catalogue);

/* Add a unique item and advance the deterministic catalogue revision. */
UmiStatus umi_digital_asset_network_catalogue_add(UmiDigitalNetworkCatalogue *catalogue, const UmiDigitalNetworkDescriptor *item);

/* Resolve an item by its stable Framework identifier. */
const UmiDigitalNetworkDescriptor *umi_digital_asset_network_catalogue_find(const UmiDigitalNetworkCatalogue *catalogue, const char *id);

#ifdef __cplusplus
}
#endif

#endif
