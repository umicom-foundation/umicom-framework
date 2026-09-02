/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_support.h
 *
 * PURPOSE:
 *   Provide focused fixtures for productisation execution control-plane tests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PRODUCT_EXECUTION_SUPPORT_H
#define UMICOM_TEST_PRODUCT_EXECUTION_SUPPORT_H
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "umicom/application/productisation/execution/execution.h"

/**
 * Exercise test step and return a clear result when the behaviour no longer matches its
 * contract.
 */
static inline UmiProductisationCompletionStep test_step(
    UmiProductisationStage stage,
    UmiProductisationSeverity severity,
    UmiProductisationOwner owner,
    UmiProductisationGapKind gap_kind,
    const char *asset)
{
    UmiProductisationCompletionStep step;
    (void)memset(&step, 0, sizeof(step));
    step.stage = stage;
    step.severity = severity;
    step.owner = owner;
    step.gap_kind = gap_kind;
    (void)snprintf(step.application_id, sizeof(step.application_id), "%s",
                   owner == UMI_PRODUCTISATION_OWNER_FRAMEWORK ? "framework" : "studio");
    (void)snprintf(step.asset_id, sizeof(step.asset_id), "%s", asset);
    (void)snprintf(step.capability_id, sizeof(step.capability_id), "cap.%s", asset);
    (void)snprintf(step.action, sizeof(step.action), "Complete %s", asset);
    return step;
}
/**
 * Exercise test plan two stage and return a clear result when the behaviour no longer
 * matches its contract.
 */
static inline UmiProductisationCompletionPlan test_plan_two_stage(void)
{
    UmiProductisationCompletionPlan plan;
    (void)memset(&plan, 0, sizeof(plan));
    plan.steps[0] = test_step(UMI_PRODUCTISATION_STAGE_FRAMEWORK,
        UMI_PRODUCTISATION_SEVERITY_BLOCKER, UMI_PRODUCTISATION_OWNER_FRAMEWORK,
        UMI_PRODUCTISATION_GAP_MISSING_CAPABILITY, "framework.core");
    plan.steps[1] = test_step(UMI_PRODUCTISATION_STAGE_STUDIO,
        UMI_PRODUCTISATION_SEVERITY_HIGH, UMI_PRODUCTISATION_OWNER_APPLICATION,
        UMI_PRODUCTISATION_GAP_APPLICATION_ADOPTION, "studio.adoption");
    plan.step_count = 2U;
    return plan;
}
/**
 * Exercise test success adapter and return a clear result when the behaviour no longer
 * matches its contract.
 */
static inline UmiStatus test_success_adapter(
    void *context,
    const UmiProductExecutionWorkItem *item,
    char *evidence,
    size_t capacity,
    UmiProductExecutionOutcome *outcome)
{
    (void)context;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL || evidence == NULL || capacity == 0U || outcome == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)snprintf(evidence, capacity, "evidence:%zu", item->plan_index);
    *outcome = UMI_PRODUCT_EXECUTION_OUTCOME_SUCCESS;
    return UMI_STATUS_OK;
}
/**
 * Exercise test retry adapter and return a clear result when the behaviour no longer
 * matches its contract.
 */
static inline UmiStatus test_retry_adapter(
    void *context,
    const UmiProductExecutionWorkItem *item,
    char *evidence,
    size_t capacity,
    UmiProductExecutionOutcome *outcome)
{
    (void)context; (void)item;
    (void)snprintf(evidence, capacity, "retry");
    *outcome = UMI_PRODUCT_EXECUTION_OUTCOME_RETRYABLE_FAILURE;
    return UMI_STATUS_IO_ERROR;
}
/**
 * Exercise test adapter and return a clear result when the behaviour no longer matches its
 * contract.
 */
static inline UmiProductExecutionAdapter test_adapter(void)
{
    UmiProductExecutionAdapter adapter;
    (void)memset(&adapter, 0, sizeof(adapter));
    adapter.framework = test_success_adapter;
    adapter.application = test_success_adapter;
    adapter.acceptance = test_success_adapter;
    return adapter;
}
#endif
