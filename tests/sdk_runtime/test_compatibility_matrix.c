/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_compatibility_matrix.c
 *
 * PURPOSE:
 *   Verify the compatibility matrix SDK/runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/sdk_runtime/compatibility_matrix.h"
int main(void)
{
    UmiSdkRuntimeCompatibilityMatrix value,same;
    umi_sdk_runtime_compatibility_matrix_init(&value,"sdk-runtime.compatibility_matrix");
    assert(umi_sdk_runtime_compatibility_matrix_validate(&value)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_compatibility_matrix_set_path(&value,"lib/umicom")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_compatibility_matrix_set_detail(&value,"installed evidence")==UMI_STATUS_OK);
    assert(umi_sdk_runtime_compatibility_matrix_set_row_count(&value,8U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_compatibility_matrix_set_failure_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_sdk_runtime_compatibility_matrix_set_state(&value,UMI_SDK_RUNTIME_STATE_READY)==UMI_STATUS_OK);
    umi_sdk_runtime_compatibility_matrix_init(&same,"sdk-runtime.compatibility_matrix");
    assert(umi_sdk_runtime_compatibility_matrix_same_identity(&value,&same));
    return 0;
    }
