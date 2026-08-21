/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/package_manifest.h
 *
 * PURPOSE:
 *   Describe one coherent SDK/runtime package and installed content.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_PACKAGE_MANIFEST
#define UMICOM_SDK_RUNTIME_PACKAGE_MANIFEST
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
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
void umi_sdk_runtime_package_manifest_init(UmiSdkRuntimePackageManifest *value, const char *id);
UmiStatus umi_sdk_runtime_package_manifest_validate(const UmiSdkRuntimePackageManifest *value);
UmiStatus umi_sdk_runtime_package_manifest_set_path(UmiSdkRuntimePackageManifest *value, const char *path);
UmiStatus umi_sdk_runtime_package_manifest_set_detail(UmiSdkRuntimePackageManifest *value, const char *detail);
UmiStatus umi_sdk_runtime_package_manifest_set_component_count(UmiSdkRuntimePackageManifest *value, uint64_t number);
UmiStatus umi_sdk_runtime_package_manifest_set_dependency_count(UmiSdkRuntimePackageManifest *value, uint64_t number);
UmiStatus umi_sdk_runtime_package_manifest_set_state(UmiSdkRuntimePackageManifest *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_package_manifest_same_identity(const UmiSdkRuntimePackageManifest *left, const UmiSdkRuntimePackageManifest *right);
#ifdef __cplusplus
}
#endif
#endif
