/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/symbol_package.h
 *
 * PURPOSE:
 *   debug symbol package metadata and build-id matching.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_SYMBOL_PACKAGE_H
#define UMICOM_DISTRIBUTION_RUNTIME_SYMBOL_PACKAGE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the dr symbol package data shared with callers of this public contract.
 */
typedef struct UmiDrSymbolPackage { char id[UMI_DR_ID_CAPACITY]; char build_id[UMI_DR_DIGEST_CAPACITY]; char digest[UMI_DR_DIGEST_CAPACITY]; uint64_t size_bytes; } UmiDrSymbolPackage;
/**
 * Initialise dr symbol package from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_symbol_package_init(UmiDrSymbolPackage *value);
/**
 * Check that dr symbol package satisfies its contract before another service relies on it.
 */
bool umi_dr_symbol_package_valid(const UmiDrSymbolPackage *value);
/**
 * Provide the dr symbol package fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_dr_symbol_package_fingerprint(const UmiDrSymbolPackage *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_dr_symbol_package_archive_encode(const UmiDrSymbolPackage *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_dr_symbol_package_archive_decode(const void *bytes, size_t byte_count,
    UmiDrSymbolPackage *value);

#ifdef __cplusplus
}
#endif
#endif
