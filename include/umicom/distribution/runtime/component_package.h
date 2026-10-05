/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/component_package.h
 *
 * PURPOSE:
 *   optional Framework component package and dependency metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_COMPONENT_PACKAGE_H
#define UMICOM_DISTRIBUTION_RUNTIME_COMPONENT_PACKAGE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr component package data shared with callers of this public contract.
 */
typedef struct UmiDrComponentPackage { char id[UMI_DR_ID_CAPACITY]; char component_id[UMI_DR_ID_CAPACITY]; UmiDrVersion version; size_t dependency_count; bool optional; } UmiDrComponentPackage;
/**
 * Initialise dr component package from caller-provided values so later operations receive
 * a known state.
 */
void umi_dr_component_package_init(UmiDrComponentPackage *value);
/**
 * Check that dr component package satisfies its contract before another service relies on
 * it.
 */
bool umi_dr_component_package_valid(const UmiDrComponentPackage *value);
/**
 * Provide the dr component package fingerprint operation used by this module and its
 * client applications.
 */
uint64_t umi_dr_component_package_fingerprint(const UmiDrComponentPackage *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_dr_component_package_archive_encode(const UmiDrComponentPackage *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_dr_component_package_archive_decode(const void *bytes, size_t byte_count,
    UmiDrComponentPackage *value);

#ifdef __cplusplus
}
#endif
#endif
