/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_loader_search.c
 *
 * PURPOSE:
 *   Verify the loader search SDK/runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/loader_search.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSdkRuntimeLoaderSearch value,same;
    umi_sdk_runtime_loader_search_init(&value,"sdk-runtime.loader_search");
    assert(umi_sdk_runtime_loader_search_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_loader_search_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_loader_search_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_loader_search_set_candidate_count(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_loader_search_set_resolved_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_loader_search_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_loader_search_init(&same,"sdk-runtime.loader_search");
    assert(umi_sdk_runtime_loader_search_same_identity(&value,&same));
    return 0;
    }
