/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/package_profile.h
 *
 * PURPOSE:
 *   named package profile selecting format, scope, compression and symbols policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_PACKAGE_PROFILE_H
#define UMICOM_DISTRIBUTION_RUNTIME_PACKAGE_PROFILE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr package profile data shared with callers of this public contract.
 */
typedef struct UmiDrPackageProfile { char id[UMI_DR_ID_CAPACITY]; UmiDrPackageFormat format; UmiDrInstallScope scope; uint32_t compression_level; bool include_symbols; bool deterministic; } UmiDrPackageProfile;
/**
 * Initialise dr package profile from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_package_profile_init(UmiDrPackageProfile *value);
/**
 * Check that dr package profile satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_package_profile_valid(const UmiDrPackageProfile *value);
/**
 * Provide the dr package profile fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_package_profile_fingerprint(const UmiDrPackageProfile *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_dr_package_profile_archive_encode(const UmiDrPackageProfile *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_dr_package_profile_archive_decode(const void *bytes, size_t byte_count,
    UmiDrPackageProfile *value);

#ifdef __cplusplus
}
#endif
#endif
