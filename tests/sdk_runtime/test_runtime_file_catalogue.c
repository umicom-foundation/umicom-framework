/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_runtime_file_catalogue.c
 *
 * PURPOSE:
 *   Verify the runtime file catalogue SDK/runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/runtime_file_catalogue.h"
int main(void)
{
    UmiSdkRuntimeRuntimeFileCatalogue value,same;
    umi_sdk_runtime_runtime_file_catalogue_init(&value,"sdk-runtime.runtime_file_catalogue");
    assert(umi_sdk_runtime_runtime_file_catalogue_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_file_catalogue_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_file_catalogue_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_file_catalogue_set_file_count(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_file_catalogue_set_generation(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_runtime_file_catalogue_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_runtime_file_catalogue_init(&same,"sdk-runtime.runtime_file_catalogue");
    assert(umi_sdk_runtime_runtime_file_catalogue_same_identity(&value,&same));
    return 0;
    }
