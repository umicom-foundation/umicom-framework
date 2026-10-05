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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_DEPOSIT_ADDRESS_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_DEPOSIT_ADDRESS_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the digital deposit address data shared with callers of this public contract.
 */
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

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_digital_asset_deposit_address_archive_encode(const UmiDigitalDepositAddress *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_digital_asset_deposit_address_archive_decode(const void *bytes, size_t byte_count,
    UmiDigitalDepositAddress *value);

#ifdef __cplusplus
}
#endif

#endif
