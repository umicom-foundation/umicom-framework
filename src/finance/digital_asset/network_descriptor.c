/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/network_descriptor.c
 *
 * PURPOSE:
 *   Implement provider-neutral blockchain or distributed-ledger network metadata.
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

#include "umicom/finance/digital_asset/network_descriptor.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_network_descriptor_init(UmiDigitalNetworkDescriptor *value, const char *id, const char *name, UmiDigitalNetworkFamily family, uint32_t minimum_confirmations)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || family == UMI_DIGITAL_NETWORK_UNKNOWN) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->id.value, sizeof value->id.value, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->name, sizeof value->name, name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->family = family;
    value->minimum_confirmations = minimum_confirmations;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_network_descriptor_valid(const UmiDigitalNetworkDescriptor *value)
{
    return value != NULL && (umi_digital_asset_text_valid(value->id.value) && umi_digital_asset_text_valid(value->name) && value->family != UMI_DIGITAL_NETWORK_UNKNOWN && value->active);
}
