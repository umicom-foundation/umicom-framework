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
UmiStatus umi_package_manifest_init(UmiPackageManifest *manifest,
                                        const char *product_id,
                                        const char *version,
                                        const char *entrypoint,
                                        UmiPackageFormat format);
/**
 * Add package manifest only after its inputs and available capacity have been checked.
 */
UmiStatus umi_package_manifest_add(UmiPackageManifest *manifest,
                                       const char *source,
                                       const char *destination,
                                       UmiPackageFileRole role,
                                       int required);
/**
 * Find package manifest while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiPackageFile *umi_package_manifest_find(
    const UmiPackageManifest *manifest,
    const char *destination);
/**
 * Check that package manifest satisfies its contract before another service relies on it.
 */
UmiStatus umi_package_manifest_validate(const UmiPackageManifest *manifest);

#ifdef __cplusplus
}
#endif
#endif
