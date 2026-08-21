/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/relocation.h
 *
 * PURPOSE:
 *   Validate relocatable paths after installation or package extraction.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_RELOCATION
#define UMICOM_SDK_RUNTIME_RELOCATION
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeRelocation
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t checked_count;
    uint64_t absolute_path_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
    } UmiSdkRuntimeRelocation;
void umi_sdk_runtime_relocation_init(UmiSdkRuntimeRelocation *value,const char *id);
UmiStatus umi_sdk_runtime_relocation_validate(const UmiSdkRuntimeRelocation *value);
UmiStatus umi_sdk_runtime_relocation_set_path(UmiSdkRuntimeRelocation *value,const char *path);
UmiStatus umi_sdk_runtime_relocation_set_detail(UmiSdkRuntimeRelocation *value,const char *detail);
UmiStatus umi_sdk_runtime_relocation_set_checked_count(UmiSdkRuntimeRelocation *value,uint64_t number);
UmiStatus umi_sdk_runtime_relocation_set_absolute_path_count(UmiSdkRuntimeRelocation *value,uint64_t number);
UmiStatus umi_sdk_runtime_relocation_set_state(UmiSdkRuntimeRelocation *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_relocation_same_identity(const UmiSdkRuntimeRelocation *left,const UmiSdkRuntimeRelocation *right);
#ifdef __cplusplus
}
#endif
#endif
