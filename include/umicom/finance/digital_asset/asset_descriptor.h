/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/asset_descriptor.h
 *
 * PURPOSE:
 *   Define a fungible digital asset and the network on which it is represented.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_ASSET_DESCRIPTOR_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_ASSET_DESCRIPTOR_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the digital asset descriptor data shared with callers of this public contract.
 */
typedef struct UmiDigitalAssetDescriptor {
    UmiDigitalAssetId id;
    char symbol[UMI_DIGITAL_ASSET_SYMBOL_CAPACITY];
    char name[UMI_DIGITAL_ASSET_NAME_CAPACITY];
    UmiDigitalAssetId network_id;
    uint32_t decimals;
    bool native_asset;
    bool active;
} UmiDigitalAssetDescriptor;

/* Initialise a bounded asset descriptor record for reusable Framework workflows. */
UmiStatus umi_digital_asset_asset_descriptor_init(UmiDigitalAssetDescriptor *value, const char *id, const char *symbol, const char *name, const char *network_id, uint32_t decimals, bool native_asset);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_digital_asset_asset_descriptor_valid(const UmiDigitalAssetDescriptor *value);

#ifdef __cplusplus
}
#endif

#endif
