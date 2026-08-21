/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_resource_lock.c
 *
 * PURPOSE:
 *   Verify the resource lock runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/resource_lock.h"
int main(void)
{
    UmiTestRuntimeResourceLock value,same;
    uint64_t r;
    umi_test_runtime_resource_lock_init(&value,"test-runtime.resource_lock");
    assert(umi_test_runtime_resource_lock_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_resource_lock_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_resource_lock_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_resource_lock_set_acquired(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_resource_lock_set_waiter_count(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_resource_lock_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.acquired==13U&&value.waiter_count==21U);
    umi_test_runtime_resource_lock_init(&same,"test-runtime.resource_lock");
    assert(umi_test_runtime_resource_lock_same_identity(&value,&same));
    return 0;
    }
