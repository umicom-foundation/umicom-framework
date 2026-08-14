/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/package_manifest_v2.h
 *
 * PURPOSE:
 *   Define deployable product files independently from an archive or installer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DELIVERY_PACKAGE_MANIFEST_V2_H
#define UMICOM_DELIVERY_PACKAGE_MANIFEST_V2_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiPackageFileRoleV2 {
    UMI_PACKAGE_FILE_EXECUTABLE_V2 = 1,
    UMI_PACKAGE_FILE_LIBRARY_V2 = 2,
    UMI_PACKAGE_FILE_RESOURCE_V2 = 3,
    UMI_PACKAGE_FILE_CONFIGURATION_V2 = 4,
    UMI_PACKAGE_FILE_LICENCE_V2 = 5,
    UMI_PACKAGE_FILE_DOCUMENTATION_V2 = 6
} UmiPackageFileRoleV2;

typedef struct UmiPackageFileV2 {
    char source[UMI_DELIVERY_PATH_CAPACITY];
    char destination[UMI_DELIVERY_PATH_CAPACITY];
    UmiPackageFileRoleV2 role;
    int required;
} UmiPackageFileV2;

typedef struct UmiPackageManifestV2 {
    char product_id[UMI_DELIVERY_ID_CAPACITY];
    char version[UMI_DELIVERY_VERSION_CAPACITY];
    char entrypoint[UMI_DELIVERY_PATH_CAPACITY];
    UmiPackageFormat format;
    UmiPackageFileV2 files[UMI_DELIVERY_MAX_ARTIFACTS];
    size_t count;
} UmiPackageManifestV2;

UmiStatus umi_package_manifest_v2_init(UmiPackageManifestV2 *manifest,
                                        const char *product_id,
                                        const char *version,
                                        const char *entrypoint,
                                        UmiPackageFormat format);
UmiStatus umi_package_manifest_v2_add(UmiPackageManifestV2 *manifest,
                                       const char *source,
                                       const char *destination,
                                       UmiPackageFileRoleV2 role,
                                       int required);
const UmiPackageFileV2 *umi_package_manifest_v2_find(
    const UmiPackageManifestV2 *manifest,
    const char *destination);
UmiStatus umi_package_manifest_v2_validate(const UmiPackageManifestV2 *manifest);

#ifdef __cplusplus
}
#endif
#endif
