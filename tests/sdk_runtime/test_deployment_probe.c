/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_deployment_probe.c
 *
 * PURPOSE:
 *   Verify the deployment probe SDK/runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/deployment_probe.h"
int main(void)
{
    UmiSdkRuntimeDeploymentProbe value,same;
    umi_sdk_runtime_deployment_probe_init(&value,"sdk-runtime.deployment_probe");
    assert(umi_sdk_runtime_deployment_probe_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_deployment_probe_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_deployment_probe_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_deployment_probe_set_checked_count(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_deployment_probe_set_failure_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_deployment_probe_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_deployment_probe_init(&same,"sdk-runtime.deployment_probe");
    assert(umi_sdk_runtime_deployment_probe_same_identity(&value,&same));
    return 0;
    }
