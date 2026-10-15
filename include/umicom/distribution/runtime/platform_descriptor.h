/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/platform_descriptor.h
 *
 * PURPOSE:
 *   runtime operating-system descriptors and minimum platform revision requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_PLATFORM_DESCRIPTOR_H
#define UMICOM_DISTRIBUTION_RUNTIME_PLATFORM_DESCRIPTOR_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr platform descriptor data shared with callers of this public contract.
 */
typedef struct UmiDrPlatformDescriptor { char id[UMI_DR_ID_CAPACITY]; UmiDrPlatform platform; UmiDrVersion minimum_version; bool desktop; bool server; } UmiDrPlatformDescriptor;
/**
 * Initialise dr platform descriptor from caller-provided values so later operations
 * receive a known state.
 */
void umi_dr_platform_descriptor_init(UmiDrPlatformDescriptor *value);
/**
 * Check that dr platform descriptor satisfies its contract before another service relies
 * on it.
 */
bool umi_dr_platform_descriptor_valid(const UmiDrPlatformDescriptor *value);
/**
 * Provide the dr platform descriptor fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_platform_descriptor_fingerprint(const UmiDrPlatformDescriptor *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_dr_platform_descriptor_archive_encode(const UmiDrPlatformDescriptor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_dr_platform_descriptor_archive_decode(const void *bytes, size_t byte_count,
    UmiDrPlatformDescriptor *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_dr_platform_descriptor_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDrPlatformDescriptor *expected, UmiDrPlatformDescriptor *value);


#ifdef __cplusplus
}
#endif
#endif
