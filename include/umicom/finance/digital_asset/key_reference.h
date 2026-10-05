/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/digital_asset/key_reference.h
 *
 * PURPOSE:
 *   Reference signing-key material by provider identifier without storing secret key bytes.
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

#ifndef INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_KEY_REFERENCE_H
#define INCLUDE_UMICOM_FINANCE_DIGITAL_ASSET_KEY_REFERENCE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/digital_asset/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the digital key reference data shared with callers of this public contract.
 */
typedef struct UmiDigitalKeyReference {
    UmiDigitalAssetId id;
    char provider_reference[UMI_DIGITAL_ASSET_NAME_CAPACITY];
    bool hardware_backed;
    bool active;
} UmiDigitalKeyReference;

/* Initialise a bounded key reference record for reusable Framework workflows. */
UmiStatus umi_digital_asset_key_reference_init(UmiDigitalKeyReference *value, const char *id, const char *provider_reference, bool hardware_backed);

/* Validate the invariant fields required before this record enters a workflow. */
bool umi_digital_asset_key_reference_valid(const UmiDigitalKeyReference *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_digital_asset_key_reference_archive_encode(const UmiDigitalKeyReference *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_digital_asset_key_reference_archive_decode(const void *bytes, size_t byte_count,
    UmiDigitalKeyReference *value);

#ifdef __cplusplus
}
#endif

#endif
