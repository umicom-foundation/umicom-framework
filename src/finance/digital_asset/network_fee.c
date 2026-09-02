/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/network_fee.c
 *
 * PURPOSE:
 *   Calculate deterministic estimated network fees from base and per-byte components.
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

#include "umicom/finance/digital_asset/network_fee.h"

#include <limits.h>
#include <string.h>

/* Store fee economics independently from any network SDK. */
UmiStatus umi_digital_asset_network_fee_init(UmiDigitalNetworkFee *value, const char *network_id, int64_t base_units, int64_t units_per_byte, int32_t scale, const char *asset_symbol)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || base_units < 0 || units_per_byte < 0 || scale < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->network_id.value, sizeof value->network_id.value, network_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->asset_symbol, sizeof value->asset_symbol, asset_symbol);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->base_units = base_units;
    value->units_per_byte = units_per_byte;
    value->scale = scale;
    return UMI_STATUS_OK;
}

/* Use integer arithmetic so estimation is deterministic and overflow checked. */
UmiStatus umi_digital_asset_network_fee_calculate(const UmiDigitalNetworkFee *value, uint32_t bytes, int64_t *out_units)
{
    int64_t byte_count = (int64_t)bytes;
    int64_t variable;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || out_units == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (byte_count != 0 && value->units_per_byte > INT64_MAX / byte_count) return UMI_STATUS_CAPACITY_EXCEEDED;
    variable = byte_count * value->units_per_byte;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (variable > INT64_MAX - value->base_units) return UMI_STATUS_CAPACITY_EXCEEDED;
    *out_units = value->base_units + variable;
    return UMI_STATUS_OK;
}
