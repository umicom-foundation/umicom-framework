/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/failure_queue.h
 *
 * PURPOSE:
 *   Maintain ordered unresolved regression failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_FAILURE_QUEUE
#define UMICOM_TEST_RUNTIME_FAILURE_QUEUE
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime failure queue data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeFailureQueue
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t pending_count;
    uint64_t generation;
    uint64_t revision;bool enabled;} UmiTestRuntimeFailureQueue;
/**
 * Initialise test runtime failure queue from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_failure_queue_init(UmiTestRuntimeFailureQueue *value,const char *id);
/**
 * Check that test runtime failure queue satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_failure_queue_validate(const UmiTestRuntimeFailureQueue *value);
/**
 * Provide the test runtime failure queue set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_failure_queue_set_detail(UmiTestRuntimeFailureQueue *value,const char *detail);
/**
 * Return the number of records represented by test runtime failure queue set pending
 * without changing their state.
 */
UmiStatus umi_test_runtime_failure_queue_set_pending_count(UmiTestRuntimeFailureQueue *value,uint64_t number);
/**
 * Provide the test runtime failure queue set generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_failure_queue_set_generation(UmiTestRuntimeFailureQueue *value,uint64_t number);
/**
 * Provide the test runtime failure queue same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_failure_queue_same_identity(const UmiTestRuntimeFailureQueue *left,const UmiTestRuntimeFailureQueue *right);
#ifdef __cplusplus
}
#endif
#endif
