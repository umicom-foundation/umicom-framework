/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/package_root.h
 *
 * PURPOSE:
 *   Describe package prefix resolution without relying on source checkout layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_PACKAGE_ROOT
#define UMICOM_SDK_RUNTIME_PACKAGE_ROOT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime package root data shared with callers of this public contract.
 */
typedef struct UmiSdkRuntimePackageRoot
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t candidate_count;
    uint64_t selected;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
    } UmiSdkRuntimePackageRoot;
/**
 * Initialise sdk runtime package root from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_package_root_init(UmiSdkRuntimePackageRoot *value,const char *id);
/**
 * Check that sdk runtime package root satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_sdk_runtime_package_root_validate(const UmiSdkRuntimePackageRoot *value);
/**
 * Provide the sdk runtime package root set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_package_root_set_path(UmiSdkRuntimePackageRoot *value,const char *path);
/**
 * Provide the sdk runtime package root set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_package_root_set_detail(UmiSdkRuntimePackageRoot *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime package root set candidate
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_package_root_set_candidate_count(UmiSdkRuntimePackageRoot *value,uint64_t number);
/**
 * Find sdk runtime package root set while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_sdk_runtime_package_root_set_selected(UmiSdkRuntimePackageRoot *value,uint64_t number);
/**
 * Provide the sdk runtime package root set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_package_root_set_state(UmiSdkRuntimePackageRoot *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime package root same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_package_root_same_identity(const UmiSdkRuntimePackageRoot *left,const UmiSdkRuntimePackageRoot *right);
#ifdef __cplusplus
}
#endif
#endif
