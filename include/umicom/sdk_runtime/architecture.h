/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/architecture.h
 *
 * PURPOSE:
 *   Describe target CPU, pointer size and ABI family.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_ARCHITECTURE
#define UMICOM_SDK_RUNTIME_ARCHITECTURE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeArchitecture
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t pointer_bits;
    uint64_t architecture_id;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
    } UmiSdkRuntimeArchitecture;
void umi_sdk_runtime_architecture_init(UmiSdkRuntimeArchitecture *value,const char *id);
UmiStatus umi_sdk_runtime_architecture_validate(const UmiSdkRuntimeArchitecture *value);
UmiStatus umi_sdk_runtime_architecture_set_path(UmiSdkRuntimeArchitecture *value,const char *path);
UmiStatus umi_sdk_runtime_architecture_set_detail(UmiSdkRuntimeArchitecture *value,const char *detail);
UmiStatus umi_sdk_runtime_architecture_set_pointer_bits(UmiSdkRuntimeArchitecture *value,uint64_t number);
UmiStatus umi_sdk_runtime_architecture_set_architecture_id(UmiSdkRuntimeArchitecture *value,uint64_t number);
UmiStatus umi_sdk_runtime_architecture_set_state(UmiSdkRuntimeArchitecture *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_architecture_same_identity(const UmiSdkRuntimeArchitecture *left,const UmiSdkRuntimeArchitecture *right);
#ifdef __cplusplus
}
#endif
#endif
