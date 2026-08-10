/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/workflow.c
 *
 * PURPOSE:
 *   Implement ordered workflow execution and reverse compensation after the first failed step.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

UmiStatus umi_workflow_create(const char *workflow_id,
                              uint32_t version,
                              size_t capacity,
                              UmiWorkflow **out_workflow)
{
    UmiWorkflow *workflow;
    if (workflow_id == NULL || workflow_id[0] == '\0' || version == 0U ||
        capacity == 0U || out_workflow == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_workflow = NULL;
    workflow = (UmiWorkflow *)calloc(1U, sizeof(*workflow));
    if (workflow == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    workflow->steps = (UmiWorkflowStep *)calloc(capacity,
                                                 sizeof(*workflow->steps));
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

void umi_workflow_destroy(UmiWorkflow *workflow)
{
    if (workflow == NULL) return;
    free(workflow->steps);
    free(workflow);
}

UmiStatus umi_workflow_add_step(UmiWorkflow *workflow,
                                const char *step_id,
                                UmiWorkflowAction action,
                                UmiWorkflowCompensation compensation,
                                void *user_data)
{
    UmiWorkflowStep *step;
    if (workflow == NULL || step_id == NULL || action == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
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

UmiWorkflowResult umi_workflow_execute(const UmiWorkflow *workflow,
                                       void *workflow_context)
{
    UmiWorkflowResult result;
    size_t index;
    result.status = UMI_STATUS_INVALID_ARGUMENT;
    result.completed_steps = 0U;
    result.compensated_steps = 0U;
    if (workflow == NULL) return result;
    result.status = UMI_STATUS_OK;
    for (index = 0U; index < workflow->count; ++index) {
        result.status = workflow->steps[index].action(
            workflow_context,
            workflow->steps[index].user_data);
        if (result.status != UMI_STATUS_OK) break;
        result.completed_steps++;
    }
    if (result.status != UMI_STATUS_OK) {
        size_t completed = result.completed_steps;
        while (completed > 0U) {
            UmiWorkflowStep *step = &workflow->steps[completed - 1U];
            if (step->compensation != NULL) {
                step->compensation(workflow_context, step->user_data);
                result.compensated_steps++;
            }
            completed--;
        }
    }
    return result;
}

const char *umi_workflow_id(const UmiWorkflow *workflow)
{
    return workflow != NULL ? workflow->id : "";
}

uint32_t umi_workflow_version(const UmiWorkflow *workflow)
{
    return workflow != NULL ? workflow->version : 0U;
}
