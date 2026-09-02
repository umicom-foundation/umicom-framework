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

/**
 * Represent the portable bundle plan data shared with callers of this public contract.
 */
typedef struct UmiPortableBundlePlan {
    char bundle_id[UMI_DELIVERY_ID_CAPACITY];
    char platform_id[UMI_DELIVERY_ID_CAPACITY];
    char staging_root[UMI_DELIVERY_PATH_CAPACITY];
    char output_path[UMI_DELIVERY_PATH_CAPACITY];
    UmiPackageManifest manifest;
    UmiRuntimeDependencyReport dependencies;
} UmiPortableBundlePlan;

/**
 * Initialise portable bundle from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_portable_bundle_init(UmiPortableBundlePlan *plan,
                                       const char *bundle_id,
                                       const char *platform_id,
                                       const char *staging_root,
                                       const char *output_path,
                                       const UmiPackageManifest *manifest);
/**
 * Provide the portable bundle add dependency operation used by this module and its client
 * applications.
 */
UmiStatus umi_portable_bundle_add_dependency(
    UmiPortableBundlePlan *plan,
    const char *name,
    const char *resolved_path,
    UmiDependencyDisposition disposition,
    int resolved);
/**
 * Check that portable bundle satisfies its contract before another service relies on it.
 */
UmiStatus umi_portable_bundle_validate(const UmiPortableBundlePlan *plan);
/**
 * Return the number of records represented by portable bundle file without changing their
 * state.
 */
size_t umi_portable_bundle_file_count(const UmiPortableBundlePlan *plan);

#ifdef __cplusplus
}
#endif
#endif
