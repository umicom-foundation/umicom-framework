/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_adapter_root.c
 *
 * PURPOSE:
 *   Verify the adapter root SDK/runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/adapter_root.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSdkRuntimeAdapterRoot value,same;
    umi_sdk_runtime_adapter_root_init(&value,"sdk-runtime.adapter_root");
    assert(umi_sdk_runtime_adapter_root_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_adapter_root_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_adapter_root_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_adapter_root_set_adapter_count(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_adapter_root_set_generation(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_adapter_root_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_adapter_root_init(&same,"sdk-runtime.adapter_root");
    assert(umi_sdk_runtime_adapter_root_same_identity(&value,&same));
    return 0;
    }
