/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_package_generation.c
 *
 * PURPOSE:
 *   Verify the package generation SDK/runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/package_generation.h"
int main(void)
{
    UmiSdkRuntimePackageGeneration value,same;
    umi_sdk_runtime_package_generation_init(&value,"sdk-runtime.package_generation");
    assert(umi_sdk_runtime_package_generation_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_generation_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_generation_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_generation_set_generation(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_generation_set_source_revision(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_package_generation_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_package_generation_init(&same,"sdk-runtime.package_generation");
    assert(umi_sdk_runtime_package_generation_same_identity(&value,&same));
    return 0;
    }
