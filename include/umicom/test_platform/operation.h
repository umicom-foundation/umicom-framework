/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/operation.h
 *
 * PURPOSE:
 *   Plan run-all, selected, rerun-failed and repeated operations while exposing
 *   a small stop controller suitable for Studio, CI and other frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_OPERATION_H
#define UMICOM_TEST_PLATFORM_OPERATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/test_platform/filter.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiTestPlatformOperationKind {
    UMI_TEST_PLATFORM_OPERATION_RUN_ALL = 0,
    UMI_TEST_PLATFORM_OPERATION_RUN_SELECTED = 1,
    UMI_TEST_PLATFORM_OPERATION_RERUN_FAILED = 2,
    UMI_TEST_PLATFORM_OPERATION_REPEAT = 3
} UmiTestPlatformOperationKind;

typedef struct UmiTestPlatformOperationPlan {
    UmiTestPlatformOperationKind kind;
    UmiTestPlatformSelection selection;
    uint32_t repeat_count;
    int stop_on_failure;
    uint64_t generation;
} UmiTestPlatformOperationPlan;

typedef struct UmiTestPlatformOperationController {
    /* Process providers translate this cooperative state into cancellation. */
    int running;
    int stop_requested;
    UmiTestPlatformOperationKind kind;
    size_t planned_execution_count;
    size_t completed_execution_count;
    uint64_t generation;
} UmiTestPlatformOperationController;

void umi_test_platform_operation_plan_init(
    UmiTestPlatformOperationPlan *plan,
    UmiTestPlatformOperationKind kind
);
UmiStatus umi_test_platform_operation_plan_all(
    UmiTestPlatformOperationPlan *plan,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformFilter *filter
);
UmiStatus umi_test_platform_operation_plan_failed(
    UmiTestPlatformOperationPlan *plan,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results
);
UmiStatus umi_test_platform_operation_plan_add(
    UmiTestPlatformOperationPlan *plan,
    const char *item_id
);
size_t umi_test_platform_operation_execution_count(
    const UmiTestPlatformOperationPlan *plan
);
void umi_test_platform_operation_controller_init(
    UmiTestPlatformOperationController *controller
);
UmiStatus umi_test_platform_operation_begin(
    UmiTestPlatformOperationController *controller,
    const UmiTestPlatformOperationPlan *plan
);
UmiStatus umi_test_platform_operation_request_stop(
    UmiTestPlatformOperationController *controller
);
int umi_test_platform_operation_should_stop(
    const UmiTestPlatformOperationController *controller
);
void umi_test_platform_operation_mark_completed(
    UmiTestPlatformOperationController *controller
);
void umi_test_platform_operation_finish(
    UmiTestPlatformOperationController *controller
);

#ifdef __cplusplus
}
#endif
#endif
