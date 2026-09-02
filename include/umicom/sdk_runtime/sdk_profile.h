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
/**
 * Represent the sdk runtime sdk profile data shared with callers of this public contract.
 */
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
/**
 * Initialise sdk runtime sdk profile from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_sdk_profile_init(UmiSdkRuntimeSdkProfile *value, const char *id);
/**
 * Check that sdk runtime sdk profile satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_sdk_runtime_sdk_profile_validate(const UmiSdkRuntimeSdkProfile *value);
/**
 * Provide the sdk runtime sdk profile set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_sdk_profile_set_path(UmiSdkRuntimeSdkProfile *value, const char *path);
/**
 * Provide the sdk runtime sdk profile set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_sdk_profile_set_detail(UmiSdkRuntimeSdkProfile *value, const char *detail);
/**
 * Return the number of records represented by sdk runtime sdk profile set required without
 * changing their state.
 */
UmiStatus umi_sdk_runtime_sdk_profile_set_required_count(UmiSdkRuntimeSdkProfile *value, uint64_t number);
/**
 * Return the number of records represented by sdk runtime sdk profile set optional without
 * changing their state.
 */
UmiStatus umi_sdk_runtime_sdk_profile_set_optional_count(UmiSdkRuntimeSdkProfile *value, uint64_t number);
/**
 * Provide the sdk runtime sdk profile set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_sdk_profile_set_state(UmiSdkRuntimeSdkProfile *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime sdk profile same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_sdk_profile_same_identity(const UmiSdkRuntimeSdkProfile *left, const UmiSdkRuntimeSdkProfile *right);
#ifdef __cplusplus
}
#endif
#endif
