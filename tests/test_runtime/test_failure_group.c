/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_failure_group.c
 *
 * PURPOSE:
 *   Verify the failure group runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/failure_group.h"
int main(void)
{
    UmiTestRuntimeFailureGroup value,same;
    uint64_t r;
    umi_test_runtime_failure_group_init(&value,"test-runtime.failure_group");
    assert(umi_test_runtime_failure_group_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_failure_group_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_failure_group_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_failure_group_set_member_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_failure_group_set_generation(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_failure_group_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.member_count==13U&&value.generation==21U);
    umi_test_runtime_failure_group_init(&same,"test-runtime.failure_group");
    assert(umi_test_runtime_failure_group_same_identity(&value,&same));
    return 0;
    }
