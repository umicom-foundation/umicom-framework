/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/runtime_dependency.h
 *
 * PURPOSE:
 *   native runtime-library dependency and availability policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_RUNTIME_DEPENDENCY_H
#define UMICOM_DISTRIBUTION_RUNTIME_RUNTIME_DEPENDENCY_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr runtime dependency data shared with callers of this public contract.
 */
typedef struct UmiDrRuntimeDependency { char id[UMI_DR_ID_CAPACITY]; char soname[UMI_DR_PATH_CAPACITY]; UmiDrVersion minimum_version; bool system_provided; bool bundled; } UmiDrRuntimeDependency;
/**
 * Initialise dr runtime dependency from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_runtime_dependency_init(UmiDrRuntimeDependency *value);
/**
 * Check that dr runtime dependency satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_runtime_dependency_valid(const UmiDrRuntimeDependency *value);
/**
 * Provide the dr runtime dependency fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_runtime_dependency_fingerprint(const UmiDrRuntimeDependency *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_dr_runtime_dependency_archive_encode(const UmiDrRuntimeDependency *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_dr_runtime_dependency_archive_decode(const void *bytes, size_t byte_count,
    UmiDrRuntimeDependency *value);

#ifdef __cplusplus
}
#endif
#endif
