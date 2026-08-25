/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/deposit_address.h
 *
 * PURPOSE:
 *   Assign a verified network address for deposits into a custody account.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_DEPOSIT_ADDRESS_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_DEPOSIT_ADDRESS_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDigitalDepositAddress {
    UmiDigitalAssetId account_id;
    UmiDigitalAssetId asset_id;
    UmiDigitalAssetId network_id;
    char address[UMI_DIGITAL_ASSET_ADDRESS_CAPACITY];
    bool active;
} UmiDigitalDepositAddress;

/* Initialise a bounded deposit address record for reusable Framework workflows. */
UmiStatus umi_digital_asset_deposit_address_init(UmiDigitalDepositAddress *value, const char *account_id, const char *asset_id, const char *network_id, const char *address);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_digital_asset_deposit_address_valid(const UmiDigitalDepositAddress *value);

#ifdef __cplusplus
}
#endif

#endif
