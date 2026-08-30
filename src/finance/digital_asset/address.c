/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/address.c
 *
 * PURPOSE:
 *   Implement a network-qualified external or custody address without embedding network SDK types.
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

#include "umicom/finance/digital_asset/address.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_address_init(UmiDigitalAssetAddress *value, const char *network_id, const char *address, bool verified)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->network_id.value, sizeof value->network_id.value, network_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->value, sizeof value->value, address);
    if (status != UMI_STATUS_OK) return status;
    value->verified = verified;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_address_valid(const UmiDigitalAssetAddress *value)
{
    return value != NULL && (umi_digital_asset_text_valid(value->network_id.value) && umi_digital_asset_text_valid(value->value));
}
