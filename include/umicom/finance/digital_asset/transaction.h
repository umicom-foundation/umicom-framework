/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/transaction.h
 *
 * PURPOSE:
 *   Define a provider-neutral on-chain transaction and confirmation evidence.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_TRANSACTION_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_TRANSACTION_H

#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDigitalAssetTransaction {
    UmiDigitalAssetId id;
    UmiDigitalAssetId network_id;
    char from_address[UMI_DIGITAL_ASSET_ADDRESS_CAPACITY];
    char to_address[UMI_DIGITAL_ASSET_ADDRESS_CAPACITY];
    UmiDigitalAmount amount;
    char transaction_hash[UMI_DIGITAL_ASSET_HASH_CAPACITY];
    UmiDigitalTransactionState state;
    uint32_t confirmations;
} UmiDigitalAssetTransaction;

/* Initialise a bounded transaction record for reusable Framework workflows. */
UmiStatus umi_digital_asset_transaction_init(UmiDigitalAssetTransaction *value, const char *id, const char *network_id, const char *from_address, const char *to_address, int64_t units, int32_t scale, const char *asset_symbol);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_digital_asset_transaction_valid(const UmiDigitalAssetTransaction *value);

#ifdef __cplusplus
}
#endif

#endif
