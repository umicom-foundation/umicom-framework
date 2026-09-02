/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/abi_requirement.h
 *
 * PURPOSE:
 *   Describe minimum and maximum ABI compatibility accepted by a consumer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_ABI_REQUIREMENT
#define UMICOM_SDK_RUNTIME_ABI_REQUIREMENT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime abi requirement data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeAbiRequirement {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t minimum_abi;
    uint64_t maximum_abi;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeAbiRequirement;
/**
 * Initialise sdk runtime abi requirement from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_abi_requirement_init(UmiSdkRuntimeAbiRequirement *value, const char *id);
/**
 * Check that sdk runtime abi requirement satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_abi_requirement_validate(const UmiSdkRuntimeAbiRequirement *value);
/**
 * Provide the sdk runtime abi requirement set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_abi_requirement_set_path(UmiSdkRuntimeAbiRequirement *value, const char *path);
/**
 * Provide the sdk runtime abi requirement set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_abi_requirement_set_detail(UmiSdkRuntimeAbiRequirement *value, const char *detail);
/**
 * Provide the sdk runtime abi requirement set minimum abi operation used by this module
 * and its client applications.
 */
UmiStatus umi_sdk_runtime_abi_requirement_set_minimum_abi(UmiSdkRuntimeAbiRequirement *value, uint64_t number);
/**
 * Provide the sdk runtime abi requirement set maximum abi operation used by this module
 * and its client applications.
 */
UmiStatus umi_sdk_runtime_abi_requirement_set_maximum_abi(UmiSdkRuntimeAbiRequirement *value, uint64_t number);
/**
 * Provide the sdk runtime abi requirement set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_abi_requirement_set_state(UmiSdkRuntimeAbiRequirement *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime abi requirement same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_abi_requirement_same_identity(const UmiSdkRuntimeAbiRequirement *left, const UmiSdkRuntimeAbiRequirement *right);
#ifdef __cplusplus
}
#endif
#endif
