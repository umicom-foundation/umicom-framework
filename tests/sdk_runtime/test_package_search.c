/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_package_search.c
 *
 * PURPOSE:
 *   Verify the package search SDK/runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/package_search.h"
int main(void)
{
    UmiSdkRuntimePackageSearch value,same;
    umi_sdk_runtime_package_search_init(&value,"sdk-runtime.package_search");
    assert(umi_sdk_runtime_package_search_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_search_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_search_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_search_set_candidate_count(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_search_set_matched_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_search_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_package_search_init(&same,"sdk-runtime.package_search");
    assert(umi_sdk_runtime_package_search_same_identity(&value,&same));
    return 0;
    }
