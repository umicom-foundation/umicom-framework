/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/resource_manifest.h
 *
 * PURPOSE:
 *   resource-pack identity, locale, scale and content metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_RESOURCE_MANIFEST_H
#define UMICOM_DISTRIBUTION_RUNTIME_RESOURCE_MANIFEST_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr resource manifest data shared with callers of this public contract.
 */
typedef struct UmiDrResourceManifest { char id[UMI_DR_ID_CAPACITY]; char locale[32]; uint32_t scale_percent; uint64_t size_bytes; char digest[UMI_DR_DIGEST_CAPACITY]; } UmiDrResourceManifest;
/**
 * Initialise dr resource manifest from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_resource_manifest_init(UmiDrResourceManifest *value);
/**
 * Check that dr resource manifest satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_resource_manifest_valid(const UmiDrResourceManifest *value);
/**
 * Provide the dr resource manifest fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_resource_manifest_fingerprint(const UmiDrResourceManifest *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_dr_resource_manifest_archive_encode(const UmiDrResourceManifest *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_dr_resource_manifest_archive_decode(const void *bytes, size_t byte_count,
    UmiDrResourceManifest *value);

#ifdef __cplusplus
}
#endif
#endif
