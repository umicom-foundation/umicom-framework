/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/dependency_manifest.h
 *
 * PURPOSE:
 *   Describe runtime dependencies required by a binary or adapter.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_DEPENDENCY_MANIFEST
#define UMICOM_SDK_RUNTIME_DEPENDENCY_MANIFEST
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeDependencyManifest {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t dependency_count;
    uint64_t missing_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeDependencyManifest;
void umi_sdk_runtime_dependency_manifest_init(UmiSdkRuntimeDependencyManifest *value, const char *id);
UmiStatus umi_sdk_runtime_dependency_manifest_validate(const UmiSdkRuntimeDependencyManifest *value);
UmiStatus umi_sdk_runtime_dependency_manifest_set_path(UmiSdkRuntimeDependencyManifest *value, const char *path);
UmiStatus umi_sdk_runtime_dependency_manifest_set_detail(UmiSdkRuntimeDependencyManifest *value, const char *detail);
UmiStatus umi_sdk_runtime_dependency_manifest_set_dependency_count(UmiSdkRuntimeDependencyManifest *value, uint64_t number);
UmiStatus umi_sdk_runtime_dependency_manifest_set_missing_count(UmiSdkRuntimeDependencyManifest *value, uint64_t number);
UmiStatus umi_sdk_runtime_dependency_manifest_set_state(UmiSdkRuntimeDependencyManifest *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_dependency_manifest_same_identity(const UmiSdkRuntimeDependencyManifest *left, const UmiSdkRuntimeDependencyManifest *right);
#ifdef __cplusplus
}
#endif
#endif
