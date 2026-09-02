/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_resource_group.c
 *
 * PURPOSE:
 *   Verify the resource group runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/resource_group.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimeResourceGroup value,same;
    uint64_t r;
    umi_test_runtime_resource_group_init(&value,"test-runtime.resource_group");
    assert(umi_test_runtime_resource_group_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_resource_group_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_resource_group_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_resource_group_set_member_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_resource_group_set_capacity(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_resource_group_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.member_count==13U&&value.capacity==21U);
    umi_test_runtime_resource_group_init(&same,"test-runtime.resource_group");
    assert(umi_test_runtime_resource_group_same_identity(&value,&same));
    return 0;
    }
