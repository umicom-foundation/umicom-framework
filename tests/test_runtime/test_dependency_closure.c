/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_dependency_closure.c
 *
 * PURPOSE:
 *   Verify the dependency closure runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/dependency_closure.h"
int main(void)
{
    UmiTestRuntimeDependencyClosure value,same;
    uint64_t r;
    umi_test_runtime_dependency_closure_init(&value,"test-runtime.dependency_closure");
    assert(umi_test_runtime_dependency_closure_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_dependency_closure_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_dependency_closure_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_dependency_closure_set_dependency_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_dependency_closure_set_missing_count(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_dependency_closure_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.dependency_count==13U&&value.missing_count==21U);
    umi_test_runtime_dependency_closure_init(&same,"test-runtime.dependency_closure");
    assert(umi_test_runtime_dependency_closure_same_identity(&value,&same));
    return 0;
    }
