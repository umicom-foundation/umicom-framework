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
/**
 * Represent the workflow data shared with callers of this public contract.
 */
typedef struct UmiWorkflow UmiWorkflow;

/**
 * Represent the workflow result data shared with callers of this public contract.
 */
typedef struct UmiWorkflowResult {
    UmiStatus status;
    size_t completed_steps;
    size_t compensated_steps;
} UmiWorkflowResult;

/**
 * Initialise workflow from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_workflow_create(const char *workflow_id,
                              uint32_t version,
                              size_t capacity,
                              UmiWorkflow **out_workflow);
/**
 * Release or reset state held by workflow so the same storage can be reused safely.
 */
void umi_workflow_destroy(UmiWorkflow *workflow);
/**
 * Provide the workflow add step operation used by this module and its client applications.
 */
UmiStatus umi_workflow_add_step(UmiWorkflow *workflow,
                                const char *step_id,
                                UmiWorkflowAction action,
                                UmiWorkflowCompensation compensation,
                                void *user_data);
/**
 * Perform workflow through the module contract so client applications do not duplicate its
 * policy.
 */
UmiWorkflowResult umi_workflow_execute(const UmiWorkflow *workflow,
                                       void *workflow_context);
/**
 * Provide the workflow id operation used by this module and its client applications.
 */
const char *umi_workflow_id(const UmiWorkflow *workflow);
/**
 * Provide the workflow version operation used by this module and its client applications.
 */
uint32_t umi_workflow_version(const UmiWorkflow *workflow);

#ifdef __cplusplus
}
#endif

#endif
