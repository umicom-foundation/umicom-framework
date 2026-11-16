/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/package_manifest.h
 *
 * PURPOSE:
 *   Define deployable product files independently from an archive or installer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DELIVERY_PACKAGE_MANIFEST_H
#define UMICOM_DELIVERY_PACKAGE_MANIFEST_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named package file role values accepted by this public contract.
 */
typedef enum UmiPackageFileRole {
    UMI_PACKAGE_FILE_EXECUTABLE = 1,
    UMI_PACKAGE_FILE_LIBRARY = 2,
    UMI_PACKAGE_FILE_RESOURCE = 3,
    UMI_PACKAGE_FILE_CONFIGURATION = 4,
    UMI_PACKAGE_FILE_LICENCE = 5,
    UMI_PACKAGE_FILE_DOCUMENTATION = 6
} UmiPackageFileRole;

/**
 * Represent the package file data shared with callers of this public contract.
 */
typedef struct UmiPackageFile {
    char source[UMI_DELIVERY_PATH_CAPACITY];
    char destination[UMI_DELIVERY_PATH_CAPACITY];
    UmiPackageFileRole role;
    int required;
} UmiPackageFile;

/**
 * Represent the package manifest data shared with callers of this public contract.
 */
typedef struct UmiPackageManifest {
    char product_id[UMI_DELIVERY_ID_CAPACITY];
    char version[UMI_DELIVERY_VERSION_CAPACITY];
    char entrypoint[UMI_DELIVERY_PATH_CAPACITY];
    UmiPackageFormat format;
    UmiPackageFile files[UMI_DELIVERY_MAX_ARTIFACTS];
    size_t count;
} UmiPackageManifest;

/**
 * Initialise package manifest from caller-provided values so later operations receive a
 * known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_package_manifest_init(UmiPackageManifest *manifest,
                                        const char *product_id,
                                        const char *version,
                                        const char *entrypoint,
                                        UmiPackageFormat format);
/**
 * Add package manifest only after its inputs and available capacity have been checked.
 */
/** Append a complete file record without changing any manifest byte on refusal.
 * Text inputs are readable NUL-terminated strings and may borrow live fields or
 * a prepared unused slot. Unknown roles return INVALID_ARGUMENT; malformed
 * existing storage returns INVALID_STATE. Duplicate destinations retain
 * ALREADY_EXISTS and a valid full manifest returns CAPACITY_EXCEEDED. */
UmiStatus umi_package_manifest_add(UmiPackageManifest *manifest,
                                       const char *source,
                                       const char *destination,
                                       UmiPackageFileRole role,
                                       int required);
/**
 * Find package manifest while leaving the underlying catalogue or model owned by this
 * module.
 */
/** A match borrows manifest storage. Any malformed live storage returns NULL,
 * including invalid entries after the requested match. Serialize all access. */
const UmiPackageFile *umi_package_manifest_find(
    const UmiPackageManifest *manifest,
    const char *destination);
/**
 * Check that package manifest satisfies its contract before another service relies on it.
 */
/** Validate fixed-array bounds, all text terminators, live file roles and unique
 * destinations. Empty header strings and an empty catalogue are allowed for
 * drafting. Unused slots and format readiness are ignored. NULL returns
 * INVALID_ARGUMENT; malformed owned storage returns INVALID_STATE. No I/O. */
UmiStatus umi_package_manifest_validate_storage(const UmiPackageManifest *manifest);

/** Additionally require nonempty product/version/entrypoint, a known package
 * format and a required executable entrypoint. This validates a plan, not file
 * presence, safe destination paths, copied bytes or an installed application. */
UmiStatus umi_package_manifest_validate(const UmiPackageManifest *manifest);

#ifdef __cplusplus
}
#endif
#endif
