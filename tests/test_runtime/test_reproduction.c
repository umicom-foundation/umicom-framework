/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_reproduction.c
 *
 * PURPOSE:
 *   Verify the reproduction runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/reproduction.h"
int main(void)
{
    UmiTestRuntimeReproduction value,same;
    uint64_t r;
    umi_test_runtime_reproduction_init(&value,"test-runtime.reproduction");
    assert(umi_test_runtime_reproduction_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_reproduction_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_reproduction_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_reproduction_set_step_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_reproduction_set_generation(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_reproduction_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.step_count==13U&&value.generation==21U);
    umi_test_runtime_reproduction_init(&same,"test-runtime.reproduction");
    assert(umi_test_runtime_reproduction_same_identity(&value,&same));
    return 0;
    }
