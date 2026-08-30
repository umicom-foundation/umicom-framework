/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/network_descriptor.h
 *
 * PURPOSE:
 *   Define provider-neutral blockchain or distributed-ledger network metadata.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_NETWORK_DESCRIPTOR_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_NETWORK_DESCRIPTOR_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDigitalNetworkDescriptor {
    UmiDigitalAssetId id;
    char name[UMI_DIGITAL_ASSET_NAME_CAPACITY];
    UmiDigitalNetworkFamily family;
    uint32_t minimum_confirmations;
    bool active;
} UmiDigitalNetworkDescriptor;

/* Initialise a bounded network descriptor record for reusable Framework workflows. */
UmiStatus umi_digital_asset_network_descriptor_init(UmiDigitalNetworkDescriptor *value, const char *id, const char *name, UmiDigitalNetworkFamily family, uint32_t minimum_confirmations);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_digital_asset_network_descriptor_valid(const UmiDigitalNetworkDescriptor *value);

#ifdef __cplusplus
}
#endif

#endif
