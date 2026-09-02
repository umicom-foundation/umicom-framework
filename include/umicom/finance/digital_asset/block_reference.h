/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/block_reference.h
 *
 * PURPOSE:
 *   Define an immutable network block reference used by settlement and audit evidence.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_BLOCK_REFERENCE_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_BLOCK_REFERENCE_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the digital block reference data shared with callers of this public contract.
 */
typedef struct UmiDigitalBlockReference {
    UmiDigitalAssetId network_id;
    uint64_t height;
    char block_hash[UMI_DIGITAL_ASSET_HASH_CAPACITY];
} UmiDigitalBlockReference;

/* Initialise a bounded block reference record for reusable Framework workflows. */
UmiStatus umi_digital_asset_block_reference_init(UmiDigitalBlockReference *value, const char *network_id, uint64_t height, const char *block_hash);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_digital_asset_block_reference_valid(const UmiDigitalBlockReference *value);

#ifdef __cplusplus
}
#endif

#endif
