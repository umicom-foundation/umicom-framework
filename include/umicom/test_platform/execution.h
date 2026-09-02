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

typedef UmiStatus (*UmiTestPlatformExecuteCallback)(
    const UmiTestPlatformItemSnapshot *item,
    uint32_t attempt,
    void *user_data,
    UmiTestPlatformResultSnapshot *out_result
);

/* Retain each callback result immediately so partial evidence survives stop. */
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
