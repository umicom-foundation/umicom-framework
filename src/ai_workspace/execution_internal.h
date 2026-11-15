/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/execution_internal.h
 *
 * PURPOSE:
 *   Share retained reviewed execution state across synchronous and background workspace clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_WORKSPACE_EXECUTION_INTERNAL_H
#define UMICOM_AI_WORKSPACE_EXECUTION_INTERNAL_H
#include "workspace_internal.h"

typedef struct AwExecution AwExecution;
typedef enum AwExecutionPhase {
    AW_EXECUTION_PREPARED, AW_EXECUTION_RUNNING, AW_EXECUTION_SETTLED
} AwExecutionPhase;
/* Private composition boundary. Callers serialize on the workspace owner
 * thread. A prepared execution owns the busy lease; it performs no provider
 * call and writes no RUNNING record until Start succeeds. Output is unchanged
 * on failure. ALREADY_EXISTS means this immutable job already succeeded. */
UmiStatus AwExecutionPrepare(UmiAiWorkspace *workspace, const char *job_id,
    const UmiAiWorkspaceCancellation *cancellation, AwExecution **out_execution);
UmiStatus AwExecutionStart(AwExecution *execution);
UmiStatus AwExecutionComplete(AwExecution *execution, UmiStatus operation_status, bool cancelled);
UmiStatus AwExecutionRelease(AwExecution **in_out_execution);
const UmiAiWorkspaceJob *AwExecutionJob(const AwExecution *execution);
const UmiAiRequest *AwExecutionRequest(const AwExecution *execution);
UmiAiResponse *AwExecutionResponse(AwExecution *execution);
AwExecutionPhase AwExecutionGetPhase(const AwExecution *execution);
UmiStatus AwExecutionOperationStatus(const AwExecution *execution);
UmiStatus AwExecutionPublicationStatus(const AwExecution *execution);
/* Shared frozen-binding check, implemented with job preparation. */
UmiStatus AwJobBinding(UmiAiWorkspace *workspace, const UmiAiWorkspaceJob *job, bool preparing);
#endif
