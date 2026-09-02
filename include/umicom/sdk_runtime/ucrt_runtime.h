/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/ucrt_runtime.h
 *
 * PURPOSE:
 *   Describe Windows UCRT runtime resolution and deployment evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_UCRT_RUNTIME
#define UMICOM_SDK_RUNTIME_UCRT_RUNTIME
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime ucrt runtime data shared with callers of this public contract.
 */
typedef struct UmiSdkRuntimeUcrtRuntime
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t dependency_count;
    uint64_t resolved_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
    } UmiSdkRuntimeUcrtRuntime;
/**
 * Initialise sdk runtime ucrt runtime from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_ucrt_runtime_init(UmiSdkRuntimeUcrtRuntime *value,const char *id);
/**
 * Check that sdk runtime ucrt runtime satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_sdk_runtime_ucrt_runtime_validate(const UmiSdkRuntimeUcrtRuntime *value);
/**
 * Provide the sdk runtime ucrt runtime set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_ucrt_runtime_set_path(UmiSdkRuntimeUcrtRuntime *value,const char *path);
/**
 * Provide the sdk runtime ucrt runtime set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_ucrt_runtime_set_detail(UmiSdkRuntimeUcrtRuntime *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime ucrt runtime set dependency
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_ucrt_runtime_set_dependency_count(UmiSdkRuntimeUcrtRuntime *value,uint64_t number);
/**
 * Return the number of records represented by sdk runtime ucrt runtime set resolved
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_ucrt_runtime_set_resolved_count(UmiSdkRuntimeUcrtRuntime *value,uint64_t number);
/**
 * Provide the sdk runtime ucrt runtime set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_ucrt_runtime_set_state(UmiSdkRuntimeUcrtRuntime *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime ucrt runtime same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_ucrt_runtime_same_identity(const UmiSdkRuntimeUcrtRuntime *left,const UmiSdkRuntimeUcrtRuntime *right);
#ifdef __cplusplus
}
#endif
#endif
