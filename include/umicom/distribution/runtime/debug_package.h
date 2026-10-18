/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/debug_package.h
 *
 * PURPOSE:
 *   diagnostic/debug companion package metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_DEBUG_PACKAGE_H
#define UMICOM_DISTRIBUTION_RUNTIME_DEBUG_PACKAGE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr debug package data shared with callers of this public contract.
 */
typedef struct UmiDrDebugPackage { char id[UMI_DR_ID_CAPACITY]; char application_id[UMI_DR_ID_CAPACITY]; bool symbols; bool diagnostics; bool source_maps; } UmiDrDebugPackage;
/**
 * Initialise dr debug package from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_debug_package_init(UmiDrDebugPackage *value);
/**
 * Check that dr debug package satisfies its contract before another service relies on it.
 */
bool umi_dr_debug_package_valid(const UmiDrDebugPackage *value);
/**
 * Provide the dr debug package fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_debug_package_fingerprint(const UmiDrDebugPackage *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_dr_debug_package_archive_encode(const UmiDrDebugPackage *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_dr_debug_package_archive_decode(const void *bytes, size_t byte_count,
    UmiDrDebugPackage *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiDrDebugPackage object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_dr_debug_package_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiDrDebugPackage *workspace, UmiDrDebugPackage *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_dr_debug_package_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDrDebugPackage *expected, UmiDrDebugPackage *value);


#ifdef __cplusplus
}
#endif
#endif
