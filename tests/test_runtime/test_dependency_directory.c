/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_dependency_directory.c
 *
 * PURPOSE:
 *   Verify the dependency directory runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/dependency_directory.h"
int main(void)
{
    UmiTestRuntimeDependencyDirectory value,same;
    uint64_t r;
    umi_test_runtime_dependency_directory_init(&value,"test-runtime.dependency_directory");
    assert(umi_test_runtime_dependency_directory_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_dependency_directory_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_dependency_directory_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_dependency_directory_set_priority(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_dependency_directory_set_resolved_count(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_dependency_directory_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.priority==13U&&value.resolved_count==21U);
    umi_test_runtime_dependency_directory_init(&same,"test-runtime.dependency_directory");
    assert(umi_test_runtime_dependency_directory_same_identity(&value,&same));
    return 0;
    }
