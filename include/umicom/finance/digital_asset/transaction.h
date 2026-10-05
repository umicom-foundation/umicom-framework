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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_TRANSACTION_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_TRANSACTION_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the digital asset transaction data shared with callers of this public
 * contract.
 */
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

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_digital_asset_transaction_archive_encode(const UmiDigitalAssetTransaction *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_digital_asset_transaction_archive_decode(const void *bytes, size_t byte_count,
    UmiDigitalAssetTransaction *value);

#ifdef __cplusplus
}
#endif

#endif
