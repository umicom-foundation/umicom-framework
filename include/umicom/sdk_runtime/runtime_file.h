/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/runtime_file.h
 *
 * PURPOSE:
 *   Describe one file required by an installed Framework runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_RUNTIME_FILE
#define UMICOM_SDK_RUNTIME_RUNTIME_FILE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeRuntimeFile
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t file_size;
    uint64_t checksum;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
    } UmiSdkRuntimeRuntimeFile;
void umi_sdk_runtime_runtime_file_init(UmiSdkRuntimeRuntimeFile *value,const char *id);
UmiStatus umi_sdk_runtime_runtime_file_validate(const UmiSdkRuntimeRuntimeFile *value);
UmiStatus umi_sdk_runtime_runtime_file_set_path(UmiSdkRuntimeRuntimeFile *value,const char *path);
UmiStatus umi_sdk_runtime_runtime_file_set_detail(UmiSdkRuntimeRuntimeFile *value,const char *detail);
UmiStatus umi_sdk_runtime_runtime_file_set_file_size(UmiSdkRuntimeRuntimeFile *value,uint64_t number);
UmiStatus umi_sdk_runtime_runtime_file_set_checksum(UmiSdkRuntimeRuntimeFile *value,uint64_t number);
UmiStatus umi_sdk_runtime_runtime_file_set_state(UmiSdkRuntimeRuntimeFile *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_runtime_file_same_identity(const UmiSdkRuntimeRuntimeFile *left,const UmiSdkRuntimeRuntimeFile *right);
#ifdef __cplusplus
}
#endif
#endif
