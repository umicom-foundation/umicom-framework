/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/export_target.h
 *
 * PURPOSE:
 *   Describe one exported CMake target and its public dependency surface.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_EXPORT_TARGET
#define UMICOM_SDK_RUNTIME_EXPORT_TARGET
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeExportTarget {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t dependency_count;
    uint64_t generation;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeExportTarget;
void umi_sdk_runtime_export_target_init(UmiSdkRuntimeExportTarget *value, const char *id);
UmiStatus umi_sdk_runtime_export_target_validate(const UmiSdkRuntimeExportTarget *value);
UmiStatus umi_sdk_runtime_export_target_set_path(UmiSdkRuntimeExportTarget *value, const char *path);
UmiStatus umi_sdk_runtime_export_target_set_detail(UmiSdkRuntimeExportTarget *value, const char *detail);
UmiStatus umi_sdk_runtime_export_target_set_dependency_count(UmiSdkRuntimeExportTarget *value, uint64_t number);
UmiStatus umi_sdk_runtime_export_target_set_generation(UmiSdkRuntimeExportTarget *value, uint64_t number);
UmiStatus umi_sdk_runtime_export_target_set_state(UmiSdkRuntimeExportTarget *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_export_target_same_identity(const UmiSdkRuntimeExportTarget *left, const UmiSdkRuntimeExportTarget *right);
#ifdef __cplusplus
}
#endif
#endif
