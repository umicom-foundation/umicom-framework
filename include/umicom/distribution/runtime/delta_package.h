/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/delta_package.h
 *
 * PURPOSE:
 *   delta package base/target version and savings validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_DELTA_PACKAGE_H
#define UMICOM_DISTRIBUTION_RUNTIME_DELTA_PACKAGE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr delta package data shared with callers of this public contract.
 */
typedef struct UmiDrDeltaPackage { char id[UMI_DR_ID_CAPACITY]; UmiDrVersion base_version; UmiDrVersion target_version; uint64_t full_size; uint64_t delta_size; char digest[UMI_DR_DIGEST_CAPACITY]; } UmiDrDeltaPackage;
/**
 * Initialise dr delta package from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_delta_package_init(UmiDrDeltaPackage *value);
/**
 * Check that dr delta package satisfies its contract before another service relies on it.
 */
bool umi_dr_delta_package_valid(const UmiDrDeltaPackage *value);
/**
 * Provide the dr delta package fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_delta_package_fingerprint(const UmiDrDeltaPackage *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_dr_delta_package_archive_encode(const UmiDrDeltaPackage *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_dr_delta_package_archive_decode(const void *bytes, size_t byte_count,
    UmiDrDeltaPackage *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiDrDeltaPackage object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_dr_delta_package_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiDrDeltaPackage *workspace, UmiDrDeltaPackage *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_dr_delta_package_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDrDeltaPackage *expected, UmiDrDeltaPackage *value);


#ifdef __cplusplus
}
#endif
#endif
