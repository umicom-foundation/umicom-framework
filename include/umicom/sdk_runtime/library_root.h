/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/library_root.h
 *
 * PURPOSE:
 *   Describe static/shared library roots and target mapping.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_LIBRARY_ROOT
#define UMICOM_SDK_RUNTIME_LIBRARY_ROOT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeLibraryRoot
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t library_count;
    uint64_t generation;
    uint64_t revision;UmiSdkRuntimeState state;bool enabled;} UmiSdkRuntimeLibraryRoot;
void umi_sdk_runtime_library_root_init(UmiSdkRuntimeLibraryRoot *value,const char *id);
UmiStatus umi_sdk_runtime_library_root_validate(const UmiSdkRuntimeLibraryRoot *value);
UmiStatus umi_sdk_runtime_library_root_set_path(UmiSdkRuntimeLibraryRoot *value,const char *path);
UmiStatus umi_sdk_runtime_library_root_set_detail(UmiSdkRuntimeLibraryRoot *value,const char *detail);
UmiStatus umi_sdk_runtime_library_root_set_library_count(UmiSdkRuntimeLibraryRoot *value,uint64_t number);
UmiStatus umi_sdk_runtime_library_root_set_generation(UmiSdkRuntimeLibraryRoot *value,uint64_t number);
UmiStatus umi_sdk_runtime_library_root_set_state(UmiSdkRuntimeLibraryRoot *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_library_root_same_identity(const UmiSdkRuntimeLibraryRoot *left,const UmiSdkRuntimeLibraryRoot *right);
#ifdef __cplusplus
}
#endif
#endif
