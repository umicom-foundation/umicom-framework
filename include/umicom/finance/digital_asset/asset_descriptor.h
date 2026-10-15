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
#include "umicom/base/value_archive.h"
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

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_digital_asset_asset_descriptor_archive_encode(const UmiDigitalAssetDescriptor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_digital_asset_asset_descriptor_archive_decode(const void *bytes, size_t byte_count,
    UmiDigitalAssetDescriptor *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_digital_asset_asset_descriptor_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDigitalAssetDescriptor *expected, UmiDigitalAssetDescriptor *value);


#ifdef __cplusplus
}
#endif

#endif
