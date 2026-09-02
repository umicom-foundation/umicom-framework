/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_failure_queue.c
 *
 * PURPOSE:
 *   Verify the failure queue runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/test_runtime/failure_queue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimeFailureQueue v,s;
    umi_test_runtime_failure_queue_init(&v,"test-runtime.failure_queue");
    assert(umi_test_runtime_failure_queue_validate(&v)==UMI_STATUS_OK);
    assert(umi_test_runtime_failure_queue_set_detail(&v,"regression evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_failure_queue_set_pending_count(&v,34U)==UMI_STATUS_OK);
    assert(umi_test_runtime_failure_queue_set_generation(&v,55U)==UMI_STATUS_OK);
    umi_test_runtime_failure_queue_init(&s,"test-runtime.failure_queue");
    assert(umi_test_runtime_failure_queue_same_identity(&v,&s));
    return 0;
    }
