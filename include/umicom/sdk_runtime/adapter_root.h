/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/adapter_root.h
 *
 * PURPOSE:
 *   Describe installed frontend and provider adapter roots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_ADAPTER_ROOT
#define UMICOM_SDK_RUNTIME_ADAPTER_ROOT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime adapter root data shared with callers of this public contract.
 */
typedef struct UmiSdkRuntimeAdapterRoot
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t adapter_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimeAdapterRoot;
/**
 * Initialise sdk runtime adapter root from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_adapter_root_init(UmiSdkRuntimeAdapterRoot *value,const char *id);
/**
 * Check that sdk runtime adapter root satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_sdk_runtime_adapter_root_validate(const UmiSdkRuntimeAdapterRoot *value);
/**
 * Provide the sdk runtime adapter root set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_adapter_root_set_path(UmiSdkRuntimeAdapterRoot *value,const char *path);
/**
 * Provide the sdk runtime adapter root set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_adapter_root_set_detail(UmiSdkRuntimeAdapterRoot *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime adapter root set adapter without
 * changing their state.
 */
UmiStatus umi_sdk_runtime_adapter_root_set_adapter_count(UmiSdkRuntimeAdapterRoot *value,uint64_t number);
/**
 * Provide the sdk runtime adapter root set generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_adapter_root_set_generation(UmiSdkRuntimeAdapterRoot *value,uint64_t number);
/**
 * Provide the sdk runtime adapter root set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_adapter_root_set_state(UmiSdkRuntimeAdapterRoot *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime adapter root same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_adapter_root_same_identity(const UmiSdkRuntimeAdapterRoot *left,const UmiSdkRuntimeAdapterRoot *right);
#ifdef __cplusplus
}
#endif
#endif
