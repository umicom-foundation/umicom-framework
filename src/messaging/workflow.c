/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/workflow.c
 *
 * PURPOSE:
 *   Implement ordered workflow execution and reverse compensation after the first failed step.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/workflow.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct UmiWorkflowStep {
    char id[128];
    UmiWorkflowAction action;
    UmiWorkflowCompensation compensation;
    void *user_data;
} UmiWorkflowStep;

struct UmiWorkflow {
    char id[128];
    uint32_t version;
    UmiWorkflowStep *steps;
    size_t capacity;
    size_t count;
};

/*
 * Initialise workflow from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_workflow_create(const char *workflow_id,
                              uint32_t version,
                              size_t capacity,
                              UmiWorkflow **out_workflow)
{
    UmiWorkflow *workflow;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workflow_id == NULL || workflow_id[0] == '\0' || version == 0U ||
        capacity == 0U || out_workflow == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_workflow = NULL;
    workflow = (UmiWorkflow *)calloc(1U, sizeof(*workflow));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workflow == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    workflow->steps = (UmiWorkflowStep *)calloc(capacity,
                                                 sizeof(*workflow->steps));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workflow->steps == NULL) {
        free(workflow);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    (void)snprintf(workflow->id, sizeof(workflow->id), "%s", workflow_id);
    workflow->version = version;
    workflow->capacity = capacity;
    *out_workflow = workflow;
    return UMI_STATUS_OK;
}

/* Release or reset state held by workflow so the same storage can be reused safely. */
void umi_workflow_destroy(UmiWorkflow *workflow)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workflow == NULL) return;
    free(workflow->steps);
    free(workflow);
}

/* Provide the workflow add step operation used by this module and its client applications. */
UmiStatus umi_workflow_add_step(UmiWorkflow *workflow,
                                const char *step_id,
                                UmiWorkflowAction action,
                                UmiWorkflowCompensation compensation,
                                void *user_data)
{
    UmiWorkflowStep *step;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workflow == NULL || step_id == NULL || action == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (workflow->count >= workflow->capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    step = &workflow->steps[workflow->count++];
    (void)snprintf(step->id, sizeof(step->id), "%s", step_id);
    step->action = action;
    step->compensation = compensation;
    step->user_data = user_data;
    return UMI_STATUS_OK;
}

/*
 * Perform workflow through the module contract so client applications do not duplicate its
 * policy.
 */
UmiWorkflowResult umi_workflow_execute(const UmiWorkflow *workflow,
                                       void *workflow_context)
{
    UmiWorkflowResult result;
    size_t index;
    result.status = UMI_STATUS_INVALID_ARGUMENT;
    result.completed_steps = 0U;
    result.compensated_steps = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workflow == NULL) return result;
    result.status = UMI_STATUS_OK;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workflow->count; ++index) {
        result.status = workflow->steps[index].action(
            workflow_context,
            workflow->steps[index].user_data);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (result.status != UMI_STATUS_OK) break;
        result.completed_steps++;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result.status != UMI_STATUS_OK) {
        size_t completed = result.completed_steps;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (completed > 0U) {
            UmiWorkflowStep *step = &workflow->steps[completed - 1U];
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (step->compensation != NULL) {
                step->compensation(workflow_context, step->user_data);
                result.compensated_steps++;
            }
            completed--;
        }
    }
    return result;
}

/* Provide the workflow id operation used by this module and its client applications. */
const char *umi_workflow_id(const UmiWorkflow *workflow)
{
    return workflow != NULL ? workflow->id : "";
}

/* Provide the workflow version operation used by this module and its client applications. */
uint32_t umi_workflow_version(const UmiWorkflow *workflow)
{
    return workflow != NULL ? workflow->version : 0U;
}
