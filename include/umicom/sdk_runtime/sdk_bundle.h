/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/sdk_bundle.h
 *
 * PURPOSE:
 *   Describe a complete developer SDK bundle and public tools.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_SDK_BUNDLE
#define UMICOM_SDK_RUNTIME_SDK_BUNDLE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeSdkBundle
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t header_count;
    uint64_t library_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
    } UmiSdkRuntimeSdkBundle;
void umi_sdk_runtime_sdk_bundle_init(UmiSdkRuntimeSdkBundle *value,const char *id);
UmiStatus umi_sdk_runtime_sdk_bundle_validate(const UmiSdkRuntimeSdkBundle *value);
UmiStatus umi_sdk_runtime_sdk_bundle_set_path(UmiSdkRuntimeSdkBundle *value,const char *path);
UmiStatus umi_sdk_runtime_sdk_bundle_set_detail(UmiSdkRuntimeSdkBundle *value,const char *detail);
UmiStatus umi_sdk_runtime_sdk_bundle_set_header_count(UmiSdkRuntimeSdkBundle *value,uint64_t number);
UmiStatus umi_sdk_runtime_sdk_bundle_set_library_count(UmiSdkRuntimeSdkBundle *value,uint64_t number);
UmiStatus umi_sdk_runtime_sdk_bundle_set_state(UmiSdkRuntimeSdkBundle *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_sdk_bundle_same_identity(const UmiSdkRuntimeSdkBundle *left,const UmiSdkRuntimeSdkBundle *right);
#ifdef __cplusplus
}
#endif
#endif
