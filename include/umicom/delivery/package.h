/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/package.h
 *
 * PURPOSE:
 *   Define package specifications and results without binding products to one archive or installer implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The same Studio release can be packaged as a directory, ZIP, Windows installer or another format through one contract.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_PACKAGE_H
#define INCLUDE_UMICOM_DELIVERY_PACKAGE_H

#include "umicom/base/status.h"
#include "umicom/base/value_archive.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the package spec data shared with callers of this public contract.
 */
typedef struct UmiPackageSpec {
    char package_id[UMI_DELIVERY_ID_CAPACITY];
    UmiPackageFormat format;
    char staging_root[UMI_DELIVERY_PATH_CAPACITY];
    char output_path[UMI_DELIVERY_PATH_CAPACITY];
    int include_symbols;
} UmiPackageSpec;

/**
 * Represent the package result data shared with callers of this public contract.
 */
typedef struct UmiPackageResult {
    char output_path[UMI_DELIVERY_PATH_CAPACITY];
    uint64_t size_bytes;
    size_t file_count;
    int succeeded;
} UmiPackageResult;

/**
 * Initialise package spec from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_package_spec_init(UmiPackageSpec *spec,
                                const char *package_id,
                                UmiPackageFormat format,
                                const char *staging_root,
                                const char *output_path);
/**
 * Check that package spec satisfies its contract before another service relies on it.
 */
UmiStatus umi_package_spec_validate(const UmiPackageSpec *spec);
/**
 * Initialise package result from caller-provided values so later operations receive a
 * known state.
 */
void umi_package_result_init(UmiPackageResult *result);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_package_spec_archive_encode(const UmiPackageSpec *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_package_spec_archive_decode(const void *bytes, size_t byte_count,
    UmiPackageSpec *value);

#ifdef __cplusplus
}
#endif

#endif
