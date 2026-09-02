/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/runtime_policy.h
 *
 * PURPOSE:
 *   Define source-build, installed-SDK and self-contained runtime policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_RUNTIME_POLICY
#define UMICOM_SDK_RUNTIME_RUNTIME_POLICY
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime runtime policy data shared with callers of this public
 * contract.
 */
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
/**
 * Initialise sdk runtime runtime policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_runtime_policy_init(UmiSdkRuntimeRuntimePolicy *value, const char *id);
/**
 * Check that sdk runtime runtime policy satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_runtime_policy_validate(const UmiSdkRuntimeRuntimePolicy *value);
/**
 * Provide the sdk runtime runtime policy set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_runtime_policy_set_path(UmiSdkRuntimeRuntimePolicy *value, const char *path);
/**
 * Provide the sdk runtime runtime policy set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_runtime_policy_set_detail(UmiSdkRuntimeRuntimePolicy *value, const char *detail);
/**
 * Provide the sdk runtime runtime policy set allow source operation used by this module
 * and its client applications.
 */
UmiStatus umi_sdk_runtime_runtime_policy_set_allow_source(UmiSdkRuntimeRuntimePolicy *value, uint64_t number);
/**
 * Provide the sdk runtime runtime policy set require installed operation used by this
 * module and its client applications.
 */
UmiStatus umi_sdk_runtime_runtime_policy_set_require_installed(UmiSdkRuntimeRuntimePolicy *value, uint64_t number);
/**
 * Provide the sdk runtime runtime policy set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_runtime_policy_set_state(UmiSdkRuntimeRuntimePolicy *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime runtime policy same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_runtime_policy_same_identity(const UmiSdkRuntimeRuntimePolicy *left, const UmiSdkRuntimeRuntimePolicy *right);
#ifdef __cplusplus
}
#endif
#endif
