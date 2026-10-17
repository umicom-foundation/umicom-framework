/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/execution.h
 *
 * PURPOSE:
 *   Execute planned test operations through a provider callback while retaining
 *   results and respecting repeat, stop and stop-on-failure semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_EXECUTION_H
#define UMICOM_TEST_PLATFORM_EXECUTION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/test_platform/item.h"
#include "umicom/test_platform/operation.h"
#include "umicom/test_platform/result.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test platform execution summary data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformExecutionSummary {
    size_t planned;
    size_t executed;
    size_t passed;
    size_t failed;
    size_t skipped;
    size_t cancelled;
    size_t timed_out;
    uint64_t duration_ms;
    int stopped;
} UmiTestPlatformExecutionSummary;

/**
 * Execute one selected test synchronously on the owning thread. The item is
 * borrowed only for this call; attempt starts at one. Fill out_result with
 * bounded text, the matching item identity, a finished outcome and a finite,
 * nonnegative duration in milliseconds below 2^64. Fractional milliseconds
 * are retained in the record and truncated only in the aggregate duration.
 * An empty result id asks the runner to generate one; an explicit id may
 * replace that provider's previous record for the same test. Reusing an id
 * belonging to another test is rejected. The runner owns sequence numbers.
 * A non-OK provider status is retained as failed, timed-out or cancelled
 * evidence rather than allowing a conflicting PASSED outcome.
 */
typedef UmiStatus (*UmiTestPlatformExecuteCallback)(
    const UmiTestPlatformItemSnapshot *item,
    uint32_t attempt,
    void *user_data,
    UmiTestPlatformResultSnapshot *out_result
);

/* Retain each callback result immediately so partial evidence survives stop. */
/**
 * Execute an owned copy of the validated plan and retain valid callback results.
 * The caller must serialize access to registries and the controller. The provider
 * may request cooperative stop or edit the caller's plan for a later request;
 * those plan edits do not affect this execution.
 *
 * Check the returned status as well as the summary: callback, validation and
 * storage errors are not successful test execution. The first error is returned
 * unchanged. Malformed results and retention failures stop further callbacks;
 * earlier retained evidence survives. executed counts callback invocations,
 * while outcome and duration totals count only successfully retained records.
 * stop_on_failure also stops on timeout or provider error. Cancellation stops
 * remaining work. Invalid plans, unknown items and exhausted counters are
 * rejected before the first provider callback. No build or process is launched
 * by this provider-neutral function itself.
 */

UmiStatus umi_test_platform_execute(
    const UmiTestPlatformItemRegistry *items,
    UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformOperationPlan *plan,
    UmiTestPlatformOperationController *controller,
    UmiTestPlatformExecuteCallback callback,
    void *user_data,
    UmiTestPlatformExecutionSummary *out_summary
);

#ifdef __cplusplus
}
#endif
#endif
