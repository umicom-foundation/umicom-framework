/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/runtime_bundle.h
 *
 * PURPOSE:
 *   Describe a self-contained application runtime bundle.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_RUNTIME_BUNDLE
#define UMICOM_SDK_RUNTIME_RUNTIME_BUNDLE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime runtime bundle data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeRuntimeBundle
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t file_count;
    uint64_t dependency_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
    } UmiSdkRuntimeRuntimeBundle;
/**
 * Initialise sdk runtime runtime bundle from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_runtime_bundle_init(UmiSdkRuntimeRuntimeBundle *value,const char *id);
/**
 * Check that sdk runtime runtime bundle satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_runtime_bundle_validate(const UmiSdkRuntimeRuntimeBundle *value);
/**
 * Provide the sdk runtime runtime bundle set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_runtime_bundle_set_path(UmiSdkRuntimeRuntimeBundle *value,const char *path);
/**
 * Provide the sdk runtime runtime bundle set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_runtime_bundle_set_detail(UmiSdkRuntimeRuntimeBundle *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime runtime bundle set file without
 * changing their state.
 */
UmiStatus umi_sdk_runtime_runtime_bundle_set_file_count(UmiSdkRuntimeRuntimeBundle *value,uint64_t number);
/**
 * Return the number of records represented by sdk runtime runtime bundle set dependency
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_runtime_bundle_set_dependency_count(UmiSdkRuntimeRuntimeBundle *value,uint64_t number);
/**
 * Provide the sdk runtime runtime bundle set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_runtime_bundle_set_state(UmiSdkRuntimeRuntimeBundle *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime runtime bundle same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_runtime_bundle_same_identity(const UmiSdkRuntimeRuntimeBundle *left,const UmiSdkRuntimeRuntimeBundle *right);
#ifdef __cplusplus
}
#endif
#endif
