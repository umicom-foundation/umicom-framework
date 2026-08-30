/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/portable_bundle_v2.h
 *
 * PURPOSE:
 *   Plan a portable application bundle from a package and dependency manifest.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DELIVERY_PORTABLE_BUNDLE_V2_H
#define UMICOM_DELIVERY_PORTABLE_BUNDLE_V2_H

#include "umicom/delivery/package_manifest_v2.h"
#include "umicom/delivery/runtime_dependency_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPortableBundlePlanV2 {
    char bundle_id[UMI_DELIVERY_ID_CAPACITY];
    char platform_id[UMI_DELIVERY_ID_CAPACITY];
    char staging_root[UMI_DELIVERY_PATH_CAPACITY];
    char output_path[UMI_DELIVERY_PATH_CAPACITY];
    UmiPackageManifestV2 manifest;
    UmiRuntimeDependencyReportV2 dependencies;
} UmiPortableBundlePlanV2;

UmiStatus umi_portable_bundle_v2_init(UmiPortableBundlePlanV2 *plan,
                                       const char *bundle_id,
                                       const char *platform_id,
                                       const char *staging_root,
                                       const char *output_path,
                                       const UmiPackageManifestV2 *manifest);
UmiStatus umi_portable_bundle_v2_add_dependency(
    UmiPortableBundlePlanV2 *plan,
    const char *name,
    const char *resolved_path,
    UmiDependencyDispositionV2 disposition,
    int resolved);
UmiStatus umi_portable_bundle_v2_validate(const UmiPortableBundlePlanV2 *plan);
size_t umi_portable_bundle_v2_file_count(const UmiPortableBundlePlanV2 *plan);

#ifdef __cplusplus
}
#endif
#endif
