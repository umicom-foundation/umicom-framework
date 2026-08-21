/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_runtime_file.c
 *
 * PURPOSE:
 *   Verify the runtime file SDK/runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/runtime_file.h"
int main(void)
{
    UmiSdkRuntimeRuntimeFile value,same;
    umi_sdk_runtime_runtime_file_init(&value,"sdk-runtime.runtime_file");
    assert(umi_sdk_runtime_runtime_file_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_file_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_file_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_file_set_file_size(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_file_set_checksum(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_file_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_runtime_file_init(&same,"sdk-runtime.runtime_file");
    assert(umi_sdk_runtime_runtime_file_same_identity(&value,&same));
    return 0;
    }
