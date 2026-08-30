/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_execution_order.c
 *
 * PURPOSE:
 *   Verify the execution order runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/test_runtime/execution_order.h"
int main(void)
{
    UmiTestRuntimeExecutionOrder v,s;
    umi_test_runtime_execution_order_init(&v,"test-runtime.execution_order");
    assert(umi_test_runtime_execution_order_validate(&v)==UMI_STATUS_OK);
    assert(umi_test_runtime_execution_order_set_detail(&v,"regression evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_execution_order_set_position(&v,34U)==UMI_STATUS_OK);
    assert(umi_test_runtime_execution_order_set_total(&v,55U)==UMI_STATUS_OK);
    umi_test_runtime_execution_order_init(&s,"test-runtime.execution_order");
    assert(umi_test_runtime_execution_order_same_identity(&v,&s));
    return 0;
    }
