/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_rerun_plan.c
 *
 * PURPOSE:
 *   Verify the rerun plan runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/test_runtime/rerun_plan.h"
int main(void)
{
    UmiTestRuntimeRerunPlan v,s;
    umi_test_runtime_rerun_plan_init(&v,"test-runtime.rerun_plan");
    assert(umi_test_runtime_rerun_plan_validate(&v)==UMI_STATUS_OK);
    assert(umi_test_runtime_rerun_plan_set_detail(&v,"regression evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_rerun_plan_set_test_count(&v,34U)==UMI_STATUS_OK);
    assert(umi_test_runtime_rerun_plan_set_attempt(&v,55U)==UMI_STATUS_OK);
    umi_test_runtime_rerun_plan_init(&s,"test-runtime.rerun_plan");
    assert(umi_test_runtime_rerun_plan_same_identity(&v,&s));
    return 0;
    }
