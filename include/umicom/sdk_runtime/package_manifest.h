/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/package_manifest.h
 *
 * PURPOSE:
 *   Describe one coherent SDK/runtime package and installed content.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_PACKAGE_MANIFEST
#define UMICOM_SDK_RUNTIME_PACKAGE_MANIFEST
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime package manifest data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimePackageManifest {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t component_count;
    uint64_t dependency_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimePackageManifest;
/**
 * Initialise sdk runtime package manifest from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_package_manifest_init(UmiSdkRuntimePackageManifest *value, const char *id);
/**
 * Check that sdk runtime package manifest satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_package_manifest_validate(const UmiSdkRuntimePackageManifest *value);
/**
 * Provide the sdk runtime package manifest set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_package_manifest_set_path(UmiSdkRuntimePackageManifest *value, const char *path);
/**
 * Provide the sdk runtime package manifest set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_package_manifest_set_detail(UmiSdkRuntimePackageManifest *value, const char *detail);
/**
 * Return the number of records represented by sdk runtime package manifest set component
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_package_manifest_set_component_count(UmiSdkRuntimePackageManifest *value, uint64_t number);
/**
 * Return the number of records represented by sdk runtime package manifest set dependency
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_package_manifest_set_dependency_count(UmiSdkRuntimePackageManifest *value, uint64_t number);
/**
 * Provide the sdk runtime package manifest set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_package_manifest_set_state(UmiSdkRuntimePackageManifest *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime package manifest same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_package_manifest_same_identity(const UmiSdkRuntimePackageManifest *left, const UmiSdkRuntimePackageManifest *right);
#ifdef __cplusplus
}
#endif
#endif
