/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/release_manifest.h
 *
 * PURPOSE:
 *   release identity, channel, platform matrix and artifact summary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_RELEASE_MANIFEST_H
#define UMICOM_DISTRIBUTION_RUNTIME_RELEASE_MANIFEST_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr release manifest data shared with callers of this public contract.
 */
typedef struct UmiDrReleaseManifest { char id[UMI_DR_ID_CAPACITY]; char application_id[UMI_DR_ID_CAPACITY]; UmiDrVersion version; UmiDrChannelKind channel; size_t artifact_count; size_t platform_count; uint64_t fingerprint; } UmiDrReleaseManifest;
/**
 * Initialise dr release manifest from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_release_manifest_init(UmiDrReleaseManifest *value);
/**
 * Check that dr release manifest satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_release_manifest_valid(const UmiDrReleaseManifest *value);
/**
 * Provide the dr release manifest fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_release_manifest_fingerprint(const UmiDrReleaseManifest *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_dr_release_manifest_archive_encode(const UmiDrReleaseManifest *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_dr_release_manifest_archive_decode(const void *bytes, size_t byte_count,
    UmiDrReleaseManifest *value);

#ifdef __cplusplus
}
#endif
#endif
