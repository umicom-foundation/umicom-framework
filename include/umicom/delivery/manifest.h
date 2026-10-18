/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/manifest.h
 *
 * PURPOSE:
 *   Describe an application release independently from the package technology used to distribute it.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The manifest is the stable identity card for a release and links source revision, version, channel and generation.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_MANIFEST_H
#define INCLUDE_UMICOM_DELIVERY_MANIFEST_H

#include "umicom/base/status.h"
#include "umicom/base/value_archive.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the delivery manifest data shared with callers of this public contract.
 */
typedef struct UmiDeliveryManifest {
    char application_id[UMI_DELIVERY_ID_CAPACITY];
    char release_id[UMI_DELIVERY_ID_CAPACITY];
    char version[UMI_DELIVERY_VERSION_CAPACITY];
    char generation_id[UMI_DELIVERY_ID_CAPACITY];
    char source_revision[UMI_DELIVERY_ID_CAPACITY];
    UmiReleaseChannel channel;
    uint64_t created_epoch_ms;
    size_t artifact_count;
} UmiDeliveryManifest;

/**
 * Initialise delivery manifest from caller-provided values so later operations receive a
 * known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_delivery_manifest_init(UmiDeliveryManifest *manifest,
                                     const char *application_id,
                                     const char *release_id,
                                     const char *version,
                                     UmiReleaseChannel channel);
/**
 * Provide the delivery manifest set generation operation used by this module and its
 * client applications.
 */
UmiStatus umi_delivery_manifest_set_generation(UmiDeliveryManifest *manifest,
                                               const char *generation_id);
/**
 * Provide the delivery manifest set source revision operation used by this module and its
 * client applications.
 */
UmiStatus umi_delivery_manifest_set_source_revision(UmiDeliveryManifest *manifest,
                                                    const char *revision);
/**
 * Check that delivery manifest satisfies its contract before another service relies on it.
 */
UmiStatus umi_delivery_manifest_validate(const UmiDeliveryManifest *manifest);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_delivery_manifest_archive_encode(const UmiDeliveryManifest *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_delivery_manifest_archive_decode(const void *bytes, size_t byte_count,
    UmiDeliveryManifest *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiDeliveryManifest object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_delivery_manifest_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiDeliveryManifest *workspace, UmiDeliveryManifest *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_delivery_manifest_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDeliveryManifest *expected, UmiDeliveryManifest *value);


#ifdef __cplusplus
}
#endif

#endif
