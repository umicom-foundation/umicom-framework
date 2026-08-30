/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/agent.h
 *
 * PURPOSE:
 *   Coordinate one governed repository-aware coding agent through context,
 *   model generation, reviewable patch creation, approval, apply, validation,
 *   bounded repair iterations and rollback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_AGENT_H
#define UMICOM_AI_CODING_RUNTIME_AGENT_H

#include "umicom/ai_coding_runtime/cancellation.h"
#include "umicom/ai_coding_runtime/audit.h"
#include "umicom/ai_coding_runtime/change_guard.h"
#include "umicom/ai_coding_runtime/event_queue.h"
#include "umicom/ai_coding_runtime/history.h"
#include "umicom/ai_coding_runtime/provider_executor.h"
#include "umicom/ai_coding_runtime/response.h"
#include "umicom/ai_coding_runtime/validation_feedback.h"
#include "umicom/ai_coding_runtime/validation_runner.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiCodingAgentBindings {
    UmiAiRuntime *ai_runtime;
    UmiAiCodingAssistantService *assistant;
    UmiAiCodingWorkspaceAdapter workspace;
    const UmiDeveloperExecutor *executor;
} UmiAiCodingAgentBindings;

typedef struct UmiAiCodingAgent UmiAiCodingAgent;

UmiStatus umi_ai_coding_agent_create(
    const UmiAiCodingAgentBindings *bindings,
    const UmiAiCodingRuntimeConfig *config,
    UmiAiCodingAgent **out_agent);

void umi_ai_coding_agent_destroy(UmiAiCodingAgent *agent);

UmiStatus umi_ai_coding_agent_set_validation_plan(
    UmiAiCodingAgent *agent,
    const UmiAiCodingValidationPlan *plan);

UmiStatus umi_ai_coding_agent_set_change_guard(
    UmiAiCodingAgent *agent,
    const UmiAiCodingChangeGuardPolicy *policy);

UmiStatus umi_ai_coding_agent_run(
    UmiAiCodingAgent *agent,
    const char *task_id,
    const UmiAiCodingRequest *request);

UmiStatus umi_ai_coding_agent_approve_pending(
    UmiAiCodingAgent *agent,
    const char *approved_by);

UmiStatus umi_ai_coding_agent_apply_pending(
    UmiAiCodingAgent *agent);

UmiStatus umi_ai_coding_agent_continue_repair(
    UmiAiCodingAgent *agent);

UmiStatus umi_ai_coding_agent_reject_pending(
    UmiAiCodingAgent *agent);

void umi_ai_coding_agent_cancel(UmiAiCodingAgent *agent);

UmiStatus umi_ai_coding_agent_next_event(
    UmiAiCodingAgent *agent,
    UmiAiCodingEvent *out_event);

size_t umi_ai_coding_agent_event_count(
    const UmiAiCodingAgent *agent);

UmiStatus umi_ai_coding_agent_snapshot(
    const UmiAiCodingAgent *agent,
    UmiAiCodingAgentSnapshot *out_snapshot);

const UmiAiCodingParsedResponse *umi_ai_coding_agent_last_response(
    const UmiAiCodingAgent *agent);

const UmiAiCodingValidationReport *umi_ai_coding_agent_last_validation(
    const UmiAiCodingAgent *agent);

UmiAiCodingHistory *umi_ai_coding_agent_history(UmiAiCodingAgent *agent);
UmiAiCodingAuditLog *umi_ai_coding_agent_audit(UmiAiCodingAgent *agent);

#ifdef __cplusplus
}
#endif
#endif
