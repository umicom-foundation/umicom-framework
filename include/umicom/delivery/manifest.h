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

#ifdef __cplusplus
}
#endif

#endif
