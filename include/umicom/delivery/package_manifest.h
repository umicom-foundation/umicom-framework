/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/package_manifest.h
 *
 * PURPOSE:
 *   Define deployable product files independently from an archive or installer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DELIVERY_PACKAGE_MANIFEST_H
#define UMICOM_DELIVERY_PACKAGE_MANIFEST_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiPackageFileRole {
    UMI_PACKAGE_FILE_EXECUTABLE = 1,
    UMI_PACKAGE_FILE_LIBRARY = 2,
    UMI_PACKAGE_FILE_RESOURCE = 3,
    UMI_PACKAGE_FILE_CONFIGURATION = 4,
    UMI_PACKAGE_FILE_LICENCE = 5,
    UMI_PACKAGE_FILE_DOCUMENTATION = 6
} UmiPackageFileRole;

typedef struct UmiPackageFile {
    char source[UMI_DELIVERY_PATH_CAPACITY];
    char destination[UMI_DELIVERY_PATH_CAPACITY];
    UmiPackageFileRole role;
    int required;
} UmiPackageFile;

typedef struct UmiPackageManifest {
    char product_id[UMI_DELIVERY_ID_CAPACITY];
    char version[UMI_DELIVERY_VERSION_CAPACITY];
    char entrypoint[UMI_DELIVERY_PATH_CAPACITY];
    UmiPackageFormat format;
    UmiPackageFile files[UMI_DELIVERY_MAX_ARTIFACTS];
    size_t count;
} UmiPackageManifest;

UmiStatus umi_package_manifest_init(UmiPackageManifest *manifest,
                                        const char *product_id,
                                        const char *version,
                                        const char *entrypoint,
                                        UmiPackageFormat format);
UmiStatus umi_package_manifest_add(UmiPackageManifest *manifest,
                                       const char *source,
                                       const char *destination,
                                       UmiPackageFileRole role,
                                       int required);
const UmiPackageFile *umi_package_manifest_find(
    const UmiPackageManifest *manifest,
    const char *destination);
UmiStatus umi_package_manifest_validate(const UmiPackageManifest *manifest);

#ifdef __cplusplus
}
#endif
#endif
