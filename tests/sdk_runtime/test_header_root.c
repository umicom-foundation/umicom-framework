/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_header_root.c
 *
 * PURPOSE:
 *   Verify the header root SDK/runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/header_root.h"
int main(void)
{
    UmiSdkRuntimeHeaderRoot value,same;
    umi_sdk_runtime_header_root_init(&value,"sdk-runtime.header_root");
    assert(umi_sdk_runtime_header_root_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_header_root_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_header_root_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_header_root_set_header_count(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_header_root_set_generation(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_header_root_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_header_root_init(&same,"sdk-runtime.header_root");
    assert(umi_sdk_runtime_header_root_same_identity(&value,&same));
    return 0;
    }
