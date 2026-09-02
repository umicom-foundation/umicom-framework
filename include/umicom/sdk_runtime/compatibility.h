/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/compatibility.h
 *
 * PURPOSE:
 *   Record deterministic compatibility decisions and explanatory evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_COMPATIBILITY
#define UMICOM_SDK_RUNTIME_COMPATIBILITY
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime compatibility data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeCompatibility {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t compatible;
    uint64_t reason_code;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeCompatibility;
/**
 * Initialise sdk runtime compatibility from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_compatibility_init(UmiSdkRuntimeCompatibility *value, const char *id);
/**
 * Check that sdk runtime compatibility satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_compatibility_validate(const UmiSdkRuntimeCompatibility *value);
/**
 * Provide the sdk runtime compatibility set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_compatibility_set_path(UmiSdkRuntimeCompatibility *value, const char *path);
/**
 * Provide the sdk runtime compatibility set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_compatibility_set_detail(UmiSdkRuntimeCompatibility *value, const char *detail);
/**
 * Provide the sdk runtime compatibility set compatible operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_compatibility_set_compatible(UmiSdkRuntimeCompatibility *value, uint64_t number);
/**
 * Provide the sdk runtime compatibility set reason code operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_compatibility_set_reason_code(UmiSdkRuntimeCompatibility *value, uint64_t number);
/**
 * Provide the sdk runtime compatibility set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_compatibility_set_state(UmiSdkRuntimeCompatibility *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime compatibility same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_compatibility_same_identity(const UmiSdkRuntimeCompatibility *left, const UmiSdkRuntimeCompatibility *right);
#ifdef __cplusplus
}
#endif
#endif
