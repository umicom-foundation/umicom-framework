/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/platform_profile.h
 *
 * PURPOSE:
 *   Describe Windows UCRT64, Linux and future platform runtime expectations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_PLATFORM_PROFILE
#define UMICOM_SDK_RUNTIME_PLATFORM_PROFILE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimePlatformProfile {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t architecture_bits;
    uint64_t dependency_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimePlatformProfile;
void umi_sdk_runtime_platform_profile_init(UmiSdkRuntimePlatformProfile *value, const char *id);
UmiStatus umi_sdk_runtime_platform_profile_validate(const UmiSdkRuntimePlatformProfile *value);
UmiStatus umi_sdk_runtime_platform_profile_set_path(UmiSdkRuntimePlatformProfile *value, const char *path);
UmiStatus umi_sdk_runtime_platform_profile_set_detail(UmiSdkRuntimePlatformProfile *value, const char *detail);
UmiStatus umi_sdk_runtime_platform_profile_set_architecture_bits(UmiSdkRuntimePlatformProfile *value, uint64_t number);
UmiStatus umi_sdk_runtime_platform_profile_set_dependency_count(UmiSdkRuntimePlatformProfile *value, uint64_t number);
UmiStatus umi_sdk_runtime_platform_profile_set_state(UmiSdkRuntimePlatformProfile *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_platform_profile_same_identity(const UmiSdkRuntimePlatformProfile *left, const UmiSdkRuntimePlatformProfile *right);
#ifdef __cplusplus
}
#endif
#endif
