/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/sdk_profile.h
 *
 * PURPOSE:
 *   Describe a consumer SDK profile and required bounded Framework targets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_SDK_PROFILE
#define UMICOM_SDK_RUNTIME_SDK_PROFILE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeSdkProfile {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t required_count;
    uint64_t optional_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeSdkProfile;
void umi_sdk_runtime_sdk_profile_init(UmiSdkRuntimeSdkProfile *value, const char *id);
UmiStatus umi_sdk_runtime_sdk_profile_validate(const UmiSdkRuntimeSdkProfile *value);
UmiStatus umi_sdk_runtime_sdk_profile_set_path(UmiSdkRuntimeSdkProfile *value, const char *path);
UmiStatus umi_sdk_runtime_sdk_profile_set_detail(UmiSdkRuntimeSdkProfile *value, const char *detail);
UmiStatus umi_sdk_runtime_sdk_profile_set_required_count(UmiSdkRuntimeSdkProfile *value, uint64_t number);
UmiStatus umi_sdk_runtime_sdk_profile_set_optional_count(UmiSdkRuntimeSdkProfile *value, uint64_t number);
UmiStatus umi_sdk_runtime_sdk_profile_set_state(UmiSdkRuntimeSdkProfile *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_sdk_profile_same_identity(const UmiSdkRuntimeSdkProfile *left, const UmiSdkRuntimeSdkProfile *right);
#ifdef __cplusplus
}
#endif
#endif
