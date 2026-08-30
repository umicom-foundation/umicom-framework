/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_test_label.c
 *
 * PURPOSE:
 *   Verify the test label runtime contract.
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
#include "umicom/test_runtime/test_label.h"
int main(void)
{
    UmiTestRuntimeTestLabel value,same;
    uint64_t r;
    umi_test_runtime_test_label_init(&value,"test-runtime.test_label");
    assert(umi_test_runtime_test_label_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_test_label_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_test_label_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_test_label_set_member_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_test_label_set_generation(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_test_label_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.member_count==13U&&value.generation==21U);
    umi_test_runtime_test_label_init(&same,"test-runtime.test_label");
    assert(umi_test_runtime_test_label_same_identity(&value,&same));
    return 0;
    }
