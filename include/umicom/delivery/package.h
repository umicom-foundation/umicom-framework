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
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
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

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiPackageSpec object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_package_spec_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiPackageSpec *workspace, UmiPackageSpec *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_package_spec_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiPackageSpec *expected, UmiPackageSpec *value);


#ifdef __cplusplus
}
#endif

#endif
