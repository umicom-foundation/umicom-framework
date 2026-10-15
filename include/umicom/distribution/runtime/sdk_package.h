/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/sdk_package.h
 *
 * PURPOSE:
 *   developer SDK package metadata and ABI compatibility range.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_SDK_PACKAGE_H
#define UMICOM_DISTRIBUTION_RUNTIME_SDK_PACKAGE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr sdk package data shared with callers of this public contract.
 */
typedef struct UmiDrSdkPackage { char id[UMI_DR_ID_CAPACITY]; UmiDrVersion version; UmiDrVersion minimum_abi; UmiDrVersion maximum_abi; bool headers; bool libraries; } UmiDrSdkPackage;
/**
 * Initialise dr sdk package from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_sdk_package_init(UmiDrSdkPackage *value);
/**
 * Check that dr sdk package satisfies its contract before another service relies on it.
 */
bool umi_dr_sdk_package_valid(const UmiDrSdkPackage *value);
/**
 * Provide the dr sdk package fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_sdk_package_fingerprint(const UmiDrSdkPackage *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_dr_sdk_package_archive_encode(const UmiDrSdkPackage *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_dr_sdk_package_archive_decode(const void *bytes, size_t byte_count,
    UmiDrSdkPackage *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_dr_sdk_package_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDrSdkPackage *expected, UmiDrSdkPackage *value);


#ifdef __cplusplus
}
#endif
#endif
