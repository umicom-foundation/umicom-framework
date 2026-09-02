/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/runtime_environment.h
 *
 * PURPOSE:
 *   Build the runtime environment needed by installed applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_RUNTIME_ENVIRONMENT
#define UMICOM_SDK_RUNTIME_RUNTIME_ENVIRONMENT
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime runtime environment data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeRuntimeEnvironment {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t entry_count;
    uint64_t path_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeRuntimeEnvironment;
/**
 * Initialise sdk runtime runtime environment from caller-provided values so later
 * operations receive a known state.
 */
void umi_sdk_runtime_runtime_environment_init(UmiSdkRuntimeRuntimeEnvironment *value, const char *id);
/**
 * Check that sdk runtime runtime environment satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_runtime_environment_validate(const UmiSdkRuntimeRuntimeEnvironment *value);
/**
 * Provide the sdk runtime runtime environment set path operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_runtime_environment_set_path(UmiSdkRuntimeRuntimeEnvironment *value, const char *path);
/**
 * Provide the sdk runtime runtime environment set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_runtime_environment_set_detail(UmiSdkRuntimeRuntimeEnvironment *value, const char *detail);
/**
 * Return the number of records represented by sdk runtime runtime environment set entry
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_runtime_environment_set_entry_count(UmiSdkRuntimeRuntimeEnvironment *value, uint64_t number);
/**
 * Return the number of records represented by sdk runtime runtime environment set path
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_runtime_environment_set_path_count(UmiSdkRuntimeRuntimeEnvironment *value, uint64_t number);
/**
 * Provide the sdk runtime runtime environment set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_runtime_environment_set_state(UmiSdkRuntimeRuntimeEnvironment *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime runtime environment same identity operation used by this module
 * and its client applications.
 */
bool umi_sdk_runtime_runtime_environment_same_identity(const UmiSdkRuntimeRuntimeEnvironment *left, const UmiSdkRuntimeRuntimeEnvironment *right);
#ifdef __cplusplus
}
#endif
#endif
