/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/asset_descriptor.c
 *
 * PURPOSE:
 *   Implement a fungible digital asset and the network on which it is represented.
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

#include "umicom/finance/digital_asset/asset_descriptor.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_asset_descriptor_init(UmiDigitalAssetDescriptor *value, const char *id, const char *symbol, const char *name, const char *network_id, uint32_t decimals, bool native_asset)
{
    UmiStatus status;
    if (value == NULL || decimals > 18U) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->id.value, sizeof value->id.value, id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->symbol, sizeof value->symbol, symbol);
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->name, sizeof value->name, name);
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->network_id.value, sizeof value->network_id.value, network_id);
    if (status != UMI_STATUS_OK) return status;
    value->decimals = decimals;
    value->native_asset = native_asset;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_asset_descriptor_valid(const UmiDigitalAssetDescriptor *value)
{
    return value != NULL && (umi_digital_asset_text_valid(value->id.value) && umi_digital_asset_text_valid(value->symbol) && umi_digital_asset_text_valid(value->network_id.value) && value->decimals <= 18U && value->active);
}
