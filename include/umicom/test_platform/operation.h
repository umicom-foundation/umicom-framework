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

/**
 * List the named test platform operation kind values accepted by this public contract.
 */
typedef enum UmiTestPlatformOperationKind {
    UMI_TEST_PLATFORM_OPERATION_RUN_ALL = 0,
    UMI_TEST_PLATFORM_OPERATION_RUN_SELECTED = 1,
    UMI_TEST_PLATFORM_OPERATION_RERUN_FAILED = 2,
    UMI_TEST_PLATFORM_OPERATION_REPEAT = 3
} UmiTestPlatformOperationKind;

/**
 * Represent the test platform operation plan data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformOperationPlan {
    UmiTestPlatformOperationKind kind;
    UmiTestPlatformSelection selection;
    uint32_t repeat_count;
    int stop_on_failure;
    uint64_t generation;
} UmiTestPlatformOperationPlan;

/**
 * Represent the test platform operation controller data shared with callers of this public
 * contract.
 */
typedef struct UmiTestPlatformOperationController {
    /* Process providers translate this cooperative state into cancellation. */
    int running;
    int stop_requested;
    UmiTestPlatformOperationKind kind;
    size_t planned_execution_count;
    size_t completed_execution_count;
    uint64_t generation;
} UmiTestPlatformOperationController;

/**
 * Initialise test platform operation plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_platform_operation_plan_init(
    UmiTestPlatformOperationPlan *plan,
    UmiTestPlatformOperationKind kind
);
/**
 * Provide the test platform operation plan all operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_operation_plan_all(
    UmiTestPlatformOperationPlan *plan,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results,
    const UmiTestPlatformFilter *filter
);
/**
 * Provide the test platform operation plan failed operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_operation_plan_failed(
    UmiTestPlatformOperationPlan *plan,
    const UmiTestPlatformItemRegistry *items,
    const UmiTestPlatformResultRegistry *results
);
/**
 * Add test platform operation plan only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_test_platform_operation_plan_add(
    UmiTestPlatformOperationPlan *plan,
    const char *item_id
);
/**
 * Return the number of records represented by test platform operation execution without
 * changing their state.
 */
size_t umi_test_platform_operation_execution_count(
    const UmiTestPlatformOperationPlan *plan
);
/**
 * Initialise test platform operation controller from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_platform_operation_controller_init(
    UmiTestPlatformOperationController *controller
);
/**
 * Provide the test platform operation begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_platform_operation_begin(
    UmiTestPlatformOperationController *controller,
    const UmiTestPlatformOperationPlan *plan
);
/**
 * Provide the test platform operation request stop operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_operation_request_stop(
    UmiTestPlatformOperationController *controller
);
/**
 * Provide the test platform operation should stop operation used by this module and its
 * client applications.
 */
int umi_test_platform_operation_should_stop(
    const UmiTestPlatformOperationController *controller
);
/**
 * Provide the test platform operation mark completed operation used by this module and its
 * client applications.
 */
void umi_test_platform_operation_mark_completed(
    UmiTestPlatformOperationController *controller
);
/**
 * Provide the test platform operation finish operation used by this module and its client
 * applications.
 */
void umi_test_platform_operation_finish(
    UmiTestPlatformOperationController *controller
);

#ifdef __cplusplus
}
#endif
#endif
