/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/adapter_root.h
 *
 * PURPOSE:
 *   Describe installed frontend and provider adapter roots.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_ADAPTER_ROOT
#define UMICOM_SDK_RUNTIME_ADAPTER_ROOT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeAdapterRoot
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t adapter_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimeAdapterRoot;
void umi_sdk_runtime_adapter_root_init(UmiSdkRuntimeAdapterRoot *value,const char *id);
UmiStatus umi_sdk_runtime_adapter_root_validate(const UmiSdkRuntimeAdapterRoot *value);
UmiStatus umi_sdk_runtime_adapter_root_set_path(UmiSdkRuntimeAdapterRoot *value,const char *path);
UmiStatus umi_sdk_runtime_adapter_root_set_detail(UmiSdkRuntimeAdapterRoot *value,const char *detail);
UmiStatus umi_sdk_runtime_adapter_root_set_adapter_count(UmiSdkRuntimeAdapterRoot *value,uint64_t number);
UmiStatus umi_sdk_runtime_adapter_root_set_generation(UmiSdkRuntimeAdapterRoot *value,uint64_t number);
UmiStatus umi_sdk_runtime_adapter_root_set_state(UmiSdkRuntimeAdapterRoot *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_adapter_root_same_identity(const UmiSdkRuntimeAdapterRoot *left,const UmiSdkRuntimeAdapterRoot *right);
#ifdef __cplusplus
}
#endif
#endif
