/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/export_target.h
 *
 * PURPOSE:
 *   Describe one exported CMake target and its public dependency surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_EXPORT_TARGET
#define UMICOM_SDK_RUNTIME_EXPORT_TARGET
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime export target data shared with callers of this public
 * contract.
 */
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
/**
 * Initialise sdk runtime export target from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_export_target_init(UmiSdkRuntimeExportTarget *value, const char *id);
/**
 * Check that sdk runtime export target satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_export_target_validate(const UmiSdkRuntimeExportTarget *value);
/**
 * Provide the sdk runtime export target set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_export_target_set_path(UmiSdkRuntimeExportTarget *value, const char *path);
/**
 * Provide the sdk runtime export target set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_export_target_set_detail(UmiSdkRuntimeExportTarget *value, const char *detail);
/**
 * Return the number of records represented by sdk runtime export target set dependency
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_export_target_set_dependency_count(UmiSdkRuntimeExportTarget *value, uint64_t number);
/**
 * Provide the sdk runtime export target set generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_export_target_set_generation(UmiSdkRuntimeExportTarget *value, uint64_t number);
/**
 * Provide the sdk runtime export target set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_export_target_set_state(UmiSdkRuntimeExportTarget *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime export target same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_export_target_same_identity(const UmiSdkRuntimeExportTarget *left, const UmiSdkRuntimeExportTarget *right);
#ifdef __cplusplus
}
#endif
#endif
