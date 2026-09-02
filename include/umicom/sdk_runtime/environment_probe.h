/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/environment_probe.h
 *
 * PURPOSE:
 *   Inspect PATH and package variables without mutating global process state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_ENVIRONMENT_PROBE
#define UMICOM_SDK_RUNTIME_ENVIRONMENT_PROBE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime environment probe data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeEnvironmentProbe
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t entry_count;
    uint64_t warning_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
    } UmiSdkRuntimeEnvironmentProbe;
/**
 * Initialise sdk runtime environment probe from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_environment_probe_init(UmiSdkRuntimeEnvironmentProbe *value,const char *id);
/**
 * Check that sdk runtime environment probe satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_environment_probe_validate(const UmiSdkRuntimeEnvironmentProbe *value);
/**
 * Provide the sdk runtime environment probe set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_environment_probe_set_path(UmiSdkRuntimeEnvironmentProbe *value,const char *path);
/**
 * Provide the sdk runtime environment probe set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_environment_probe_set_detail(UmiSdkRuntimeEnvironmentProbe *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime environment probe set entry
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_environment_probe_set_entry_count(UmiSdkRuntimeEnvironmentProbe *value,uint64_t number);
/**
 * Return the number of records represented by sdk runtime environment probe set warning
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_environment_probe_set_warning_count(UmiSdkRuntimeEnvironmentProbe *value,uint64_t number);
/**
 * Provide the sdk runtime environment probe set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_environment_probe_set_state(UmiSdkRuntimeEnvironmentProbe *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime environment probe same identity operation used by this module
 * and its client applications.
 */
bool umi_sdk_runtime_environment_probe_same_identity(const UmiSdkRuntimeEnvironmentProbe *left,const UmiSdkRuntimeEnvironmentProbe *right);
#ifdef __cplusplus
}
#endif
#endif
