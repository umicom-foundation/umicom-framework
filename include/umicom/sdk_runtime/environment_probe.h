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
void umi_sdk_runtime_environment_probe_init(UmiSdkRuntimeEnvironmentProbe *value,const char *id);
UmiStatus umi_sdk_runtime_environment_probe_validate(const UmiSdkRuntimeEnvironmentProbe *value);
UmiStatus umi_sdk_runtime_environment_probe_set_path(UmiSdkRuntimeEnvironmentProbe *value,const char *path);
UmiStatus umi_sdk_runtime_environment_probe_set_detail(UmiSdkRuntimeEnvironmentProbe *value,const char *detail);
UmiStatus umi_sdk_runtime_environment_probe_set_entry_count(UmiSdkRuntimeEnvironmentProbe *value,uint64_t number);
UmiStatus umi_sdk_runtime_environment_probe_set_warning_count(UmiSdkRuntimeEnvironmentProbe *value,uint64_t number);
UmiStatus umi_sdk_runtime_environment_probe_set_state(UmiSdkRuntimeEnvironmentProbe *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_environment_probe_same_identity(const UmiSdkRuntimeEnvironmentProbe *left,const UmiSdkRuntimeEnvironmentProbe *right);
#ifdef __cplusplus
}
#endif
#endif
