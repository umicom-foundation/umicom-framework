/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_architecture.c
 *
 * PURPOSE:
 *   Verify the architecture SDK/runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/architecture.h"
int main(void)
{
    UmiSdkRuntimeArchitecture value,same;
    umi_sdk_runtime_architecture_init(&value,"sdk-runtime.architecture");
    assert(umi_sdk_runtime_architecture_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_architecture_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_architecture_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_architecture_set_pointer_bits(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_architecture_set_architecture_id(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_architecture_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_architecture_init(&same,"sdk-runtime.architecture");
    assert(umi_sdk_runtime_architecture_same_identity(&value,&same));
    return 0;
    }
