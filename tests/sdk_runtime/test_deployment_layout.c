/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_deployment_layout.c
 *
 * PURPOSE:
 *   Verify the deployment layout contract and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/sdk_runtime/deployment_layout.h"
int main(void)
{
    UmiSdkRuntimeDeploymentLayout value; UmiSdkRuntimeDeploymentLayout same; uint64_t revision;
    umi_sdk_runtime_deployment_layout_init(&value, "sdk-runtime.deployment_layout");
    assert(umi_sdk_runtime_deployment_layout_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_deployment_layout_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_deployment_layout_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_deployment_layout_set_directory_count(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_deployment_layout_set_generation(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_deployment_layout_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.directory_count == 3U && value.generation == 5U);
    umi_sdk_runtime_deployment_layout_init(&same, "sdk-runtime.deployment_layout");
    assert(umi_sdk_runtime_deployment_layout_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}
