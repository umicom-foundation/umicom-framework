/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/scheduler.h
 *
 * PURPOSE:
 *   Plan bounded parallel execution without obscuring deterministic result ordering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_SCHEDULER
#define UMICOM_TEST_RUNTIME_SCHEDULER

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime scheduler data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeScheduler {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t queued_count;
    uint64_t running_count;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeScheduler;

/**
 * Initialise test runtime scheduler from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_scheduler_init(UmiTestRuntimeScheduler *value, const char *id);
/**
 * Check that test runtime scheduler satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_test_runtime_scheduler_validate(const UmiTestRuntimeScheduler *value);
/**
 * Provide the test runtime scheduler set name operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_scheduler_set_name(UmiTestRuntimeScheduler *value, const char *name);
/**
 * Provide the test runtime scheduler set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_scheduler_set_detail(UmiTestRuntimeScheduler *value, const char *detail);
/**
 * Return the number of records represented by test runtime scheduler set queued without
 * changing their state.
 */
UmiStatus umi_test_runtime_scheduler_set_queued_count(UmiTestRuntimeScheduler *value, uint64_t number);
/**
 * Return the number of records represented by test runtime scheduler set running without
 * changing their state.
 */
UmiStatus umi_test_runtime_scheduler_set_running_count(UmiTestRuntimeScheduler *value, uint64_t number);
/**
 * Provide the test runtime scheduler touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_scheduler_touch(UmiTestRuntimeScheduler *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime scheduler same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_scheduler_same_identity(const UmiTestRuntimeScheduler *left, const UmiTestRuntimeScheduler *right);

#ifdef __cplusplus
}
#endif
#endif
