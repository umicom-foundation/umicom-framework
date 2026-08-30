/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/key_reference.c
 *
 * PURPOSE:
 *   Reference signing-key material by provider identifier without storing secret key bytes.
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

#include "umicom/finance/digital_asset/key_reference.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_key_reference_init(UmiDigitalKeyReference *value, const char *id, const char *provider_reference, bool hardware_backed)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->id.value, sizeof value->id.value, id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->provider_reference, sizeof value->provider_reference, provider_reference);
    if (status != UMI_STATUS_OK) return status;
    value->hardware_backed = hardware_backed;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_key_reference_valid(const UmiDigitalKeyReference *value)
{
    return value != NULL && (umi_digital_asset_text_valid(value->id.value) && umi_digital_asset_text_valid(value->provider_reference) && value->active);
}
