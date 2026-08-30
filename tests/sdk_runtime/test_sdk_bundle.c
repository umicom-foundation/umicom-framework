/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_sdk_bundle.c
 *
 * PURPOSE:
 *   Verify the sdk bundle SDK/runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/sdk_bundle.h"
int main(void)
{
    UmiSdkRuntimeSdkBundle value,same;
    umi_sdk_runtime_sdk_bundle_init(&value,"sdk-runtime.sdk_bundle");
    assert(umi_sdk_runtime_sdk_bundle_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_sdk_bundle_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_sdk_bundle_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_sdk_bundle_set_header_count(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_sdk_bundle_set_library_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_sdk_bundle_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_sdk_bundle_init(&same,"sdk-runtime.sdk_bundle");
    assert(umi_sdk_runtime_sdk_bundle_same_identity(&value,&same));
    return 0;
    }
