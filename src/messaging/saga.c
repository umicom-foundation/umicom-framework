/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/saga.c
 *
 * PURPOSE:
 *   Implement saga initialisation, workflow execution and terminal-state classification based on completion or compensation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/saga.h"

#include <string.h>

/* Initialise saga from caller-provided values so later operations receive a known state. */
void umi_saga_init(UmiSaga *saga,
                   uint64_t saga_id,
                   uint64_t correlation_id,
                   const UmiWorkflow *workflow)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (saga == NULL) return;
    (void)memset(saga, 0, sizeof(*saga));
    saga->saga_id = saga_id;
    saga->correlation_id = correlation_id;
    saga->workflow = workflow;
    saga->state = UMI_SAGA_CREATED;
}

/*
 * Perform saga through the module contract so client applications do not duplicate its
 * policy.
 */
UmiStatus umi_saga_execute(UmiSaga *saga, void *workflow_context)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (saga == NULL || saga->workflow == NULL || saga->saga_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (saga->state != UMI_SAGA_CREATED) return UMI_STATUS_INVALID_STATE;
    saga->state = UMI_SAGA_RUNNING;
    saga->result = umi_workflow_execute(saga->workflow, workflow_context);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (saga->result.status == UMI_STATUS_OK) {
        saga->state = UMI_SAGA_COMPLETED;
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (saga->result.compensated_steps ==
               saga->result.completed_steps &&
               saga->result.completed_steps > 0U) {
        saga->state = UMI_SAGA_COMPENSATED;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        saga->state = UMI_SAGA_FAILED;
    }
    return saga->result.status;
}

/* Provide the saga state text operation used by this module and its client applications. */
const char *umi_saga_state_text(UmiSagaState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
    case UMI_SAGA_CREATED: return "created";
    case UMI_SAGA_RUNNING: return "running";
    case UMI_SAGA_COMPLETED: return "completed";
    case UMI_SAGA_COMPENSATED: return "compensated";
    case UMI_SAGA_FAILED: return "failed";
    default: return "unknown";
    }
}
