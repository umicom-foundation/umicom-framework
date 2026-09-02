/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/deployment_probe.h
 *
 * PURPOSE:
 *   Probe an installed or extracted runtime before application launch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_DEPLOYMENT_PROBE
#define UMICOM_SDK_RUNTIME_DEPLOYMENT_PROBE
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime deployment probe data shared with callers of this public
 * contract.
 */
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
/**
 * Initialise sdk runtime deployment probe from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_deployment_probe_init(UmiSdkRuntimeDeploymentProbe *value,const char *id);
/**
 * Check that sdk runtime deployment probe satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_deployment_probe_validate(const UmiSdkRuntimeDeploymentProbe *value);
/**
 * Provide the sdk runtime deployment probe set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_deployment_probe_set_path(UmiSdkRuntimeDeploymentProbe *value,const char *path);
/**
 * Provide the sdk runtime deployment probe set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_deployment_probe_set_detail(UmiSdkRuntimeDeploymentProbe *value,const char *detail);
/**
 * Return the number of records represented by sdk runtime deployment probe set checked
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_deployment_probe_set_checked_count(UmiSdkRuntimeDeploymentProbe *value,uint64_t number);
/**
 * Return the number of records represented by sdk runtime deployment probe set failure
 * without changing their state.
 */
UmiStatus umi_sdk_runtime_deployment_probe_set_failure_count(UmiSdkRuntimeDeploymentProbe *value,uint64_t number);
/**
 * Provide the sdk runtime deployment probe set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_deployment_probe_set_state(UmiSdkRuntimeDeploymentProbe *value,UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime deployment probe same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_deployment_probe_same_identity(const UmiSdkRuntimeDeploymentProbe *left,const UmiSdkRuntimeDeploymentProbe *right);
#ifdef __cplusplus
}
#endif
#endif
