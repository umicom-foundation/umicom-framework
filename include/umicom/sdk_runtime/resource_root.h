/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/resource_root.h
 *
 * PURPOSE:
 *   Describe installed shared resources independent of source-tree paths.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_RESOURCE_ROOT
#define UMICOM_SDK_RUNTIME_RESOURCE_ROOT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeResourceRoot
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t resource_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimeResourceRoot;
void umi_sdk_runtime_resource_root_init(UmiSdkRuntimeResourceRoot *value,const char *id);
UmiStatus umi_sdk_runtime_resource_root_validate(const UmiSdkRuntimeResourceRoot *value);
UmiStatus umi_sdk_runtime_resource_root_set_path(UmiSdkRuntimeResourceRoot *value,const char *path);
UmiStatus umi_sdk_runtime_resource_root_set_detail(UmiSdkRuntimeResourceRoot *value,const char *detail);
UmiStatus umi_sdk_runtime_resource_root_set_resource_count(UmiSdkRuntimeResourceRoot *value,uint64_t number);
UmiStatus umi_sdk_runtime_resource_root_set_generation(UmiSdkRuntimeResourceRoot *value,uint64_t number);
UmiStatus umi_sdk_runtime_resource_root_set_state(UmiSdkRuntimeResourceRoot *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_resource_root_same_identity(const UmiSdkRuntimeResourceRoot *left,const UmiSdkRuntimeResourceRoot *right);
#ifdef __cplusplus
}
#endif
#endif
