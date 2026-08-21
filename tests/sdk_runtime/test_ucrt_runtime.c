/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_ucrt_runtime.c
 *
 * PURPOSE:
 *   Verify the ucrt runtime SDK/runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/ucrt_runtime.h"
int main(void)
{
    UmiSdkRuntimeUcrtRuntime value,same;
    umi_sdk_runtime_ucrt_runtime_init(&value,"sdk-runtime.ucrt_runtime");
    assert(umi_sdk_runtime_ucrt_runtime_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_ucrt_runtime_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_ucrt_runtime_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_ucrt_runtime_set_dependency_count(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_ucrt_runtime_set_resolved_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_ucrt_runtime_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_ucrt_runtime_init(&same,"sdk-runtime.ucrt_runtime");
    assert(umi_sdk_runtime_ucrt_runtime_same_identity(&value,&same));
    return 0;
    }
