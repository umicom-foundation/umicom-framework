/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/workflow.h
 *
 * PURPOSE:
 *   Declare versioned workflows with execute and compensation handlers for controlled long-running application operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_WORKFLOW_H
#define UMICOM_MESSAGING_WORKFLOW_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiWorkflowAction)(void *workflow_context,
                                       void *user_data);
typedef void (*UmiWorkflowCompensation)(void *workflow_context,
                                        void *user_data);
typedef struct UmiWorkflow UmiWorkflow;

typedef struct UmiWorkflowResult {
    UmiStatus status;
    size_t completed_steps;
    size_t compensated_steps;
} UmiWorkflowResult;

UmiStatus umi_workflow_create(const char *workflow_id,
                              uint32_t version,
                              size_t capacity,
                              UmiWorkflow **out_workflow);
void umi_workflow_destroy(UmiWorkflow *workflow);
UmiStatus umi_workflow_add_step(UmiWorkflow *workflow,
                                const char *step_id,
                                UmiWorkflowAction action,
                                UmiWorkflowCompensation compensation,
                                void *user_data);
UmiWorkflowResult umi_workflow_execute(const UmiWorkflow *workflow,
                                       void *workflow_context);
const char *umi_workflow_id(const UmiWorkflow *workflow);
uint32_t umi_workflow_version(const UmiWorkflow *workflow);

#ifdef __cplusplus
}
#endif

#endif
