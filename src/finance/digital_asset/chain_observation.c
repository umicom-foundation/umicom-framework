/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/chain_observation.c
 *
 * PURPOSE:
 *   Capture point-in-time chain height and tip hash evidence for one network.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/digital_asset/chain_observation.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_chain_observation_init(UmiDigitalChainObservation *value, const char *network_id, uint64_t height, const char *block_hash, int64_t observed_time_ms)
{
    UmiStatus status;
    if (value == NULL || observed_time_ms < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->network_id.value, sizeof value->network_id.value, network_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->block_hash, sizeof value->block_hash, block_hash);
    if (status != UMI_STATUS_OK) return status;
    value->height = height;
    value->observed_time_ms = observed_time_ms;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_chain_observation_valid(const UmiDigitalChainObservation *value)
{
    return value != NULL && (umi_digital_asset_text_valid(value->network_id.value) && umi_digital_asset_text_valid(value->block_hash) && value->observed_time_ms >= 0);
}
