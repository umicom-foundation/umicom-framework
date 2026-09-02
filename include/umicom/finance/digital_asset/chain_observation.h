/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/chain_observation.h
 *
 * PURPOSE:
 *   Capture point-in-time chain height and tip hash evidence for one network.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_CHAIN_OBSERVATION_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_CHAIN_OBSERVATION_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the digital chain observation data shared with callers of this public
 * contract.
 */
typedef struct UmiDigitalChainObservation {
    UmiDigitalAssetId network_id;
    uint64_t height;
    char block_hash[UMI_DIGITAL_ASSET_HASH_CAPACITY];
    int64_t observed_time_ms;
} UmiDigitalChainObservation;

/* Initialise a bounded chain observation record for reusable Framework workflows. */
UmiStatus umi_digital_asset_chain_observation_init(UmiDigitalChainObservation *value, const char *network_id, uint64_t height, const char *block_hash, int64_t observed_time_ms);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_digital_asset_chain_observation_valid(const UmiDigitalChainObservation *value);

#ifdef __cplusplus
}
#endif

#endif
