/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/resource_root.h
 *
 * PURPOSE:
 *   Describe installed shared resources independent of source-tree paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_RESOURCE_ROOT
#define UMICOM_SDK_RUNTIME_RESOURCE_ROOT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime resource root data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeResourceRoot
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t resource_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimeResourceRoot;
/**
 * Initialise sdk runtime resource root from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_resource_root_init(UmiSdkRuntimeResourceRoot *value,const char *id);
/**
 * Check that sdk runtime resource root satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_resource_root_validate(const UmiSdkRuntimeResourceRoot *value);
/**
 * Provide the sdk runtime resource root set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_resource_root_set_path(UmiSdkRuntimeResourceRoot *value,const char *path);
/**
 * Provide the sdk runtime resource root set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_resource_root_set_detail(UmiSdkRuntimeResourceRoot *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime resource root set resource
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_resource_root_set_resource_count(UmiSdkRuntimeResourceRoot *value,uint64_t number);
/**
 * Provide the sdk runtime resource root set generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_resource_root_set_generation(UmiSdkRuntimeResourceRoot *value,uint64_t number);
/**
 * Provide the sdk runtime resource root set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_resource_root_set_state(UmiSdkRuntimeResourceRoot *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime resource root same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_resource_root_same_identity(const UmiSdkRuntimeResourceRoot *left,const UmiSdkRuntimeResourceRoot *right);
#ifdef __cplusplus
}
#endif
#endif
