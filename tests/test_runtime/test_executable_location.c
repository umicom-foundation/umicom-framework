/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_executable_location.c
 *
 * PURPOSE:
 *   Verify the executable location runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/executable_location.h"
int main(void)
{
    UmiTestRuntimeExecutableLocation value,same;
    uint64_t r;
    umi_test_runtime_executable_location_init(&value,"test-runtime.executable_location");
    assert(umi_test_runtime_executable_location_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_executable_location_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_executable_location_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_executable_location_set_priority(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_executable_location_set_exists(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_executable_location_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.priority==13U&&value.exists==21U);
    umi_test_runtime_executable_location_init(&same,"test-runtime.executable_location");
    assert(umi_test_runtime_executable_location_same_identity(&value,&same));
    return 0;
    }
