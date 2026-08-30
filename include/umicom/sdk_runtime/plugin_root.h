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
typedef struct UmiSdkRuntimePluginRoot
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t plugin_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimePluginRoot;
void umi_sdk_runtime_plugin_root_init(UmiSdkRuntimePluginRoot *value,const char *id);
UmiStatus umi_sdk_runtime_plugin_root_validate(const UmiSdkRuntimePluginRoot *value);
UmiStatus umi_sdk_runtime_plugin_root_set_path(UmiSdkRuntimePluginRoot *value,const char *path);
UmiStatus umi_sdk_runtime_plugin_root_set_detail(UmiSdkRuntimePluginRoot *value,const char *detail);
UmiStatus umi_sdk_runtime_plugin_root_set_plugin_count(UmiSdkRuntimePluginRoot *value,uint64_t number);
UmiStatus umi_sdk_runtime_plugin_root_set_generation(UmiSdkRuntimePluginRoot *value,uint64_t number);
UmiStatus umi_sdk_runtime_plugin_root_set_state(UmiSdkRuntimePluginRoot *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_plugin_root_same_identity(const UmiSdkRuntimePluginRoot *left,const UmiSdkRuntimePluginRoot *right);
#ifdef __cplusplus
}
#endif
#endif
