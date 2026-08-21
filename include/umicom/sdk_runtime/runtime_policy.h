/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/runtime_policy.h
 *
 * PURPOSE:
 *   Define source-build, installed-SDK and self-contained runtime policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_RUNTIME_POLICY
#define UMICOM_SDK_RUNTIME_RUNTIME_POLICY
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeRuntimePolicy {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t allow_source;
    uint64_t require_installed;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeRuntimePolicy;
void umi_sdk_runtime_runtime_policy_init(UmiSdkRuntimeRuntimePolicy *value, const char *id);
UmiStatus umi_sdk_runtime_runtime_policy_validate(const UmiSdkRuntimeRuntimePolicy *value);
UmiStatus umi_sdk_runtime_runtime_policy_set_path(UmiSdkRuntimeRuntimePolicy *value, const char *path);
UmiStatus umi_sdk_runtime_runtime_policy_set_detail(UmiSdkRuntimeRuntimePolicy *value, const char *detail);
UmiStatus umi_sdk_runtime_runtime_policy_set_allow_source(UmiSdkRuntimeRuntimePolicy *value, uint64_t number);
UmiStatus umi_sdk_runtime_runtime_policy_set_require_installed(UmiSdkRuntimeRuntimePolicy *value, uint64_t number);
UmiStatus umi_sdk_runtime_runtime_policy_set_state(UmiSdkRuntimeRuntimePolicy *value, UmiSdkRuntimeState state);
bool umi_sdk_runtime_runtime_policy_same_identity(const UmiSdkRuntimeRuntimePolicy *left, const UmiSdkRuntimeRuntimePolicy *right);
#ifdef __cplusplus
}
#endif
#endif
