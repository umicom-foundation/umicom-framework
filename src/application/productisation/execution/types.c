/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/types.c
 *
 * PURPOSE:
 *   Implement deterministic execution-state vocabulary and transition validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/types.h"

/*
 * Provide the product execution state text operation used by this module and its client
 * applications.
 */
const char *umi_product_execution_state_text(UmiProductExecutionState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
    case UMI_PRODUCT_EXECUTION_PENDING: return "pending";
    case UMI_PRODUCT_EXECUTION_READY: return "ready";
    case UMI_PRODUCT_EXECUTION_RUNNING: return "running";
    case UMI_PRODUCT_EXECUTION_SUCCEEDED: return "succeeded";
    case UMI_PRODUCT_EXECUTION_FAILED: return "failed";
    case UMI_PRODUCT_EXECUTION_BLOCKED: return "blocked";
    case UMI_PRODUCT_EXECUTION_CANCELLED: return "cancelled";
    case UMI_PRODUCT_EXECUTION_ROLLED_BACK: return "rolled_back";
    default: return "unknown";
    }
}
/*
 * Provide the product execution outcome text operation used by this module and its client
 * applications.
 */
const char *umi_product_execution_outcome_text(UmiProductExecutionOutcome outcome)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (outcome) {
    case UMI_PRODUCT_EXECUTION_OUTCOME_NONE: return "none";
    case UMI_PRODUCT_EXECUTION_OUTCOME_SUCCESS: return "success";
    case UMI_PRODUCT_EXECUTION_OUTCOME_RETRYABLE_FAILURE: return "retryable_failure";
    case UMI_PRODUCT_EXECUTION_OUTCOME_PERMANENT_FAILURE: return "permanent_failure";
    case UMI_PRODUCT_EXECUTION_OUTCOME_BLOCKED: return "blocked";
    case UMI_PRODUCT_EXECUTION_OUTCOME_CANCELLED: return "cancelled";
    default: return "unknown";
    }
}
/*
 * Provide the product execution event kind text operation used by this module and its
 * client applications.
 */
const char *umi_product_execution_event_kind_text(UmiProductExecutionEventKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
    case UMI_PRODUCT_EXECUTION_EVENT_QUEUED: return "queued";
    case UMI_PRODUCT_EXECUTION_EVENT_READY: return "ready";
    case UMI_PRODUCT_EXECUTION_EVENT_STARTED: return "started";
    case UMI_PRODUCT_EXECUTION_EVENT_SUCCEEDED: return "succeeded";
    case UMI_PRODUCT_EXECUTION_EVENT_FAILED: return "failed";
    case UMI_PRODUCT_EXECUTION_EVENT_BLOCKED: return "blocked";
    case UMI_PRODUCT_EXECUTION_EVENT_CANCELLED: return "cancelled";
    case UMI_PRODUCT_EXECUTION_EVENT_CHECKPOINT: return "checkpoint";
    case UMI_PRODUCT_EXECUTION_EVENT_ROLLBACK: return "rollback";
    default: return "unknown";
    }
}
/*
 * Provide the product execution state terminal operation used by this module and its
 * client applications.
 */
int umi_product_execution_state_terminal(UmiProductExecutionState state)
{
    return state == UMI_PRODUCT_EXECUTION_SUCCEEDED ||
           state == UMI_PRODUCT_EXECUTION_FAILED ||
           state == UMI_PRODUCT_EXECUTION_BLOCKED ||
           state == UMI_PRODUCT_EXECUTION_CANCELLED ||
           state == UMI_PRODUCT_EXECUTION_ROLLED_BACK;
}
/*
 * Provide the product execution state can transition operation used by this module and its
 * client applications.
 */
int umi_product_execution_state_can_transition(
    UmiProductExecutionState current,
    UmiProductExecutionState next)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (current == UMI_PRODUCT_EXECUTION_PENDING)
        return next == UMI_PRODUCT_EXECUTION_READY ||
               next == UMI_PRODUCT_EXECUTION_BLOCKED ||
               next == UMI_PRODUCT_EXECUTION_CANCELLED;
    /* Apply this operation only while the related capability or state is available. */
    if (current == UMI_PRODUCT_EXECUTION_READY)
        return next == UMI_PRODUCT_EXECUTION_RUNNING ||
               next == UMI_PRODUCT_EXECUTION_BLOCKED ||
               next == UMI_PRODUCT_EXECUTION_CANCELLED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (current == UMI_PRODUCT_EXECUTION_RUNNING)
        return next == UMI_PRODUCT_EXECUTION_SUCCEEDED ||
               next == UMI_PRODUCT_EXECUTION_FAILED ||
               next == UMI_PRODUCT_EXECUTION_BLOCKED ||
               next == UMI_PRODUCT_EXECUTION_CANCELLED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (current == UMI_PRODUCT_EXECUTION_FAILED)
        return next == UMI_PRODUCT_EXECUTION_READY ||
               next == UMI_PRODUCT_EXECUTION_ROLLED_BACK;
    /* Apply this branch only when its contract condition is satisfied. */
    if (current == UMI_PRODUCT_EXECUTION_SUCCEEDED)
        return next == UMI_PRODUCT_EXECUTION_ROLLED_BACK;
    return 0;
}
