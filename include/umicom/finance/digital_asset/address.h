/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/address.h
 *
 * PURPOSE:
 *   Define a network-qualified external or custody address without embedding network SDK types.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_ADDRESS_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_ADDRESS_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDigitalAssetAddress {
    UmiDigitalAssetId network_id;
    char value[UMI_DIGITAL_ASSET_ADDRESS_CAPACITY];
    bool verified;
} UmiDigitalAssetAddress;

/* Initialise a bounded address record for reusable Framework workflows. */
UmiStatus umi_digital_asset_address_init(UmiDigitalAssetAddress *value, const char *network_id, const char *address, bool verified);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_digital_asset_address_valid(const UmiDigitalAssetAddress *value);

#ifdef __cplusplus
}
#endif

#endif
