/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_path_normalisation.c
 *
 * PURPOSE:
 *   Verify the path normalisation SDK/runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/path_normalisation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSdkRuntimePathNormalisation value,same;
    umi_sdk_runtime_path_normalisation_init(&value,"sdk-runtime.path_normalisation");
    assert(umi_sdk_runtime_path_normalisation_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_path_normalisation_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_path_normalisation_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_path_normalisation_set_input_count(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_path_normalisation_set_changed_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_path_normalisation_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_path_normalisation_init(&same,"sdk-runtime.path_normalisation");
    assert(umi_sdk_runtime_path_normalisation_same_identity(&value,&same));
    return 0;
    }
