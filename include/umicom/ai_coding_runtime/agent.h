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

#include "umicom/ai/coding_patch.h"
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

/* A completed call into the shared patch service, correlated to its owning task
 * and patch. transaction.admitted distinguishes a full transaction report from
 * service admission refusal. status is the patch call's primary result, not the
 * result of the whole agent run or its later notification/validation steps. */
typedef struct UmiAiCodingAgentPatchOutcome {
    char task_id[UMI_AI_CODING_RUNTIME_ID_CAPACITY];
    char patch_id[UMI_AI_ID_CAPACITY];
    int reverting;
    UmiStatus status;
    UmiAiCodingPatchTransactionReport transaction;
} UmiAiCodingAgentPatchOutcome;

/* Copy the most recently completed patch call for the current admitted task.
 * NOT_FOUND means no call completed; every refusal leaves outOutcome unchanged.
 * A new admitted run clears the previous outcome. During synchronous callbacks,
 * readers see the previous completed outcome until the active patch call returns.
 * Correlate both IDs: a newly generated patch may not yet have been attempted.
 * Use separate output storage and serialize access; no ownership is transferred. */
UmiStatus UmiAiCodingAgentReadPatchOutcome(const UmiAiCodingAgent *agent,
    UmiAiCodingAgentPatchOutcome *outOutcome);


/**
 * Initialise ai coding agent from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_coding_agent_create(
    const UmiAiCodingAgentBindings *bindings,
    const UmiAiCodingRuntimeConfig *config,
    UmiAiCodingAgent **out_agent);

/** Read-only serialized lifetime preflight. NULL is idle. BUSY means an
 * agent operation or a borrowed assistant patch operation is active. This
 * observation is not a lock and does not reserve lifetime across threads. */
UmiStatus UmiAiCodingAgentCheckIdle(const UmiAiCodingAgent *agent);

/** Refuse active execution without freeing the agent. On OK the caller must clear its owner
 * handle. NULL is accepted; the legacy void destroy also retains a busy owner.
 * Borrowed runtime, assistant, executor and workspace storage must outlive the
 * complete call. Callback code must not destroy or directly mutate them.
 * Run/apply/repair and policy mutations refuse reentry. Cancellation and
 * serialized snapshot/event observation remain available during execution. */
UmiStatus UmiAiCodingAgentDestroyChecked(UmiAiCodingAgent *agent);

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
/* Transition revisions increase throughout this agent lifetime, including across
 * tasks. Starting work requires two available identities; refusal preserves the
 * prior task and recovery outcome. Within an admitted operation, the last
 * available identity records a terminal capacity failure if a requested state
 * would leave unfinished work. Live state remains authoritative if its event
 * cannot be retained. The revision does not count every intermediate field
 * assignment or independent assistant mutation; serialize reads and use task
 * identifiers and child observations as well. Retire an exhausted owner after
 * inspecting its final state; never reset its counter to reuse identities. */
/* Task IDs and fixed request text are admitted through the shared draft
 * constructor before prior task data is cleared. Overlong IDs are refused,
 * never truncated. Inputs are copied before reset, so serialized callers may
 * pass task/request storage borrowed from this owner. Full execution readiness
 * and privacy policy are still checked by assistant preparation. */
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
