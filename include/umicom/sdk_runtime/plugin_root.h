/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/plugin_root.h
 *
 * PURPOSE:
 *   Describe installed plug-in search roots and permission boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_PLUGIN_ROOT
#define UMICOM_SDK_RUNTIME_PLUGIN_ROOT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime plugin root data shared with callers of this public contract.
 */
typedef struct UmiSdkRuntimePluginRoot
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t plugin_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimePluginRoot;
/**
 * Initialise sdk runtime plugin root from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_plugin_root_init(UmiSdkRuntimePluginRoot *value,const char *id);
/**
 * Check that sdk runtime plugin root satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_sdk_runtime_plugin_root_validate(const UmiSdkRuntimePluginRoot *value);
/**
 * Provide the sdk runtime plugin root set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_plugin_root_set_path(UmiSdkRuntimePluginRoot *value,const char *path);
/**
 * Provide the sdk runtime plugin root set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_plugin_root_set_detail(UmiSdkRuntimePluginRoot *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime plugin root set plugin without
 * changing their state.
 */
UmiStatus umi_sdk_runtime_plugin_root_set_plugin_count(UmiSdkRuntimePluginRoot *value,uint64_t number);
/**
 * Provide the sdk runtime plugin root set generation operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_plugin_root_set_generation(UmiSdkRuntimePluginRoot *value,uint64_t number);
/**
 * Provide the sdk runtime plugin root set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_plugin_root_set_state(UmiSdkRuntimePluginRoot *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime plugin root same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_plugin_root_same_identity(const UmiSdkRuntimePluginRoot *left,const UmiSdkRuntimePluginRoot *right);
#ifdef __cplusplus
}
#endif
#endif
