/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/portable_bundle.h
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
#ifndef UMICOM_DELIVERY_PORTABLE_BUNDLE_H
#define UMICOM_DELIVERY_PORTABLE_BUNDLE_H

#include "umicom/delivery/package_manifest.h"
#include "umicom/delivery/runtime_dependency.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPortableBundlePlan {
    char bundle_id[UMI_DELIVERY_ID_CAPACITY];
    char platform_id[UMI_DELIVERY_ID_CAPACITY];
    char staging_root[UMI_DELIVERY_PATH_CAPACITY];
    char output_path[UMI_DELIVERY_PATH_CAPACITY];
    UmiPackageManifest manifest;
    UmiRuntimeDependencyReport dependencies;
} UmiPortableBundlePlan;

UmiStatus umi_portable_bundle_init(UmiPortableBundlePlan *plan,
                                       const char *bundle_id,
                                       const char *platform_id,
                                       const char *staging_root,
                                       const char *output_path,
                                       const UmiPackageManifest *manifest);
UmiStatus umi_portable_bundle_add_dependency(
    UmiPortableBundlePlan *plan,
    const char *name,
    const char *resolved_path,
    UmiDependencyDisposition disposition,
    int resolved);
UmiStatus umi_portable_bundle_validate(const UmiPortableBundlePlan *plan);
size_t umi_portable_bundle_file_count(const UmiPortableBundlePlan *plan);

#ifdef __cplusplus
}
#endif
#endif
