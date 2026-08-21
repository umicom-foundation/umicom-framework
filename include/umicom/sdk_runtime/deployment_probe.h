/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/deployment_probe.h
 *
 * PURPOSE:
 *   Probe an installed or extracted runtime before application launch.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_DEPLOYMENT_PROBE
#define UMICOM_SDK_RUNTIME_DEPLOYMENT_PROBE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkRuntimeDeploymentProbe
{
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t checked_count;
    uint64_t failure_count;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
    } UmiSdkRuntimeDeploymentProbe;
void umi_sdk_runtime_deployment_probe_init(UmiSdkRuntimeDeploymentProbe *value,const char *id);
UmiStatus umi_sdk_runtime_deployment_probe_validate(const UmiSdkRuntimeDeploymentProbe *value);
UmiStatus umi_sdk_runtime_deployment_probe_set_path(UmiSdkRuntimeDeploymentProbe *value,const char *path);
UmiStatus umi_sdk_runtime_deployment_probe_set_detail(UmiSdkRuntimeDeploymentProbe *value,const char *detail);
UmiStatus umi_sdk_runtime_deployment_probe_set_checked_count(UmiSdkRuntimeDeploymentProbe *value,uint64_t number);
UmiStatus umi_sdk_runtime_deployment_probe_set_failure_count(UmiSdkRuntimeDeploymentProbe *value,uint64_t number);
UmiStatus umi_sdk_runtime_deployment_probe_set_state(UmiSdkRuntimeDeploymentProbe *value,UmiSdkRuntimeState state);
bool umi_sdk_runtime_deployment_probe_same_identity(const UmiSdkRuntimeDeploymentProbe *left,const UmiSdkRuntimeDeploymentProbe *right);
#ifdef __cplusplus
}
#endif
#endif
