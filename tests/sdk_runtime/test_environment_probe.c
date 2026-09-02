/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_environment_probe.c
 *
 * PURPOSE:
 *   Verify the environment probe SDK/runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/environment_probe.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSdkRuntimeEnvironmentProbe value,same;
    umi_sdk_runtime_environment_probe_init(&value,"sdk-runtime.environment_probe");
    assert(umi_sdk_runtime_environment_probe_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_environment_probe_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_environment_probe_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_environment_probe_set_entry_count(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_environment_probe_set_warning_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_environment_probe_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_environment_probe_init(&same,"sdk-runtime.environment_probe");
    assert(umi_sdk_runtime_environment_probe_same_identity(&value,&same));
    return 0;
    }
