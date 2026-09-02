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

/**
 * Represent the ai coding agent bindings data shared with callers of this public contract.
 */
typedef struct UmiAiCodingAgentBindings {
    UmiAiRuntime *ai_runtime;
    UmiAiCodingAssistantService *assistant;
    UmiAiCodingWorkspaceAdapter workspace;
    const UmiDeveloperExecutor *executor;
} UmiAiCodingAgentBindings;

/**
 * Represent the ai coding agent data shared with callers of this public contract.
 */
typedef struct UmiAiCodingAgent UmiAiCodingAgent;

/**
 * Initialise ai coding agent from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_coding_agent_create(
    const UmiAiCodingAgentBindings *bindings,
    const UmiAiCodingRuntimeConfig *config,
    UmiAiCodingAgent **out_agent);

/**
 * Release or reset state held by ai coding agent so the same storage can be reused safely.
 */
void umi_ai_coding_agent_destroy(UmiAiCodingAgent *agent);

/**
 * Provide the ai coding agent set validation plan operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_agent_set_validation_plan(
    UmiAiCodingAgent *agent,
    const UmiAiCodingValidationPlan *plan);

/**
 * Provide the ai coding agent set change guard operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_agent_set_change_guard(
    UmiAiCodingAgent *agent,
    const UmiAiCodingChangeGuardPolicy *policy);

/**
 * Perform ai coding agent through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_ai_coding_agent_run(
    UmiAiCodingAgent *agent,
    const char *task_id,
    const UmiAiCodingRequest *request);

/**
 * Provide the ai coding agent approve pending operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_agent_approve_pending(
    UmiAiCodingAgent *agent,
    const char *approved_by);

/**
 * Provide the ai coding agent apply pending operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_agent_apply_pending(
    UmiAiCodingAgent *agent);

/**
 * Provide the ai coding agent continue repair operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_agent_continue_repair(
    UmiAiCodingAgent *agent);

/**
 * Provide the ai coding agent reject pending operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_agent_reject_pending(
    UmiAiCodingAgent *agent);

/**
 * Provide the ai coding agent cancel operation used by this module and its client
 * applications.
 */
void umi_ai_coding_agent_cancel(UmiAiCodingAgent *agent);

/**
 * Provide the ai coding agent next event operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_agent_next_event(
    UmiAiCodingAgent *agent,
    UmiAiCodingEvent *out_event);

/**
 * Return the number of records represented by ai coding agent event without changing their
 * state.
 */
size_t umi_ai_coding_agent_event_count(
    const UmiAiCodingAgent *agent);

/**
 * Provide the ai coding agent snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_agent_snapshot(
    const UmiAiCodingAgent *agent,
    UmiAiCodingAgentSnapshot *out_snapshot);

/**
 * Provide the ai coding agent last response operation used by this module and its client
 * applications.
 */
const UmiAiCodingParsedResponse *umi_ai_coding_agent_last_response(
    const UmiAiCodingAgent *agent);

/**
 * Provide the ai coding agent last validation operation used by this module and its client
 * applications.
 */
const UmiAiCodingValidationReport *umi_ai_coding_agent_last_validation(
    const UmiAiCodingAgent *agent);

/**
 * Provide the ai coding agent history operation used by this module and its client
 * applications.
 */
UmiAiCodingHistory *umi_ai_coding_agent_history(UmiAiCodingAgent *agent);
/**
 * Provide the ai coding agent audit operation used by this module and its client
 * applications.
 */
UmiAiCodingAuditLog *umi_ai_coding_agent_audit(UmiAiCodingAgent *agent);

#ifdef __cplusplus
}
#endif
#endif
