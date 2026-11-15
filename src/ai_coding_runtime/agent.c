/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/agent.c
 *
 * PURPOSE:
 *   Implement the operational coding-agent state machine. Existing coding
 *   assistant planning, privacy and patch transactions remain authoritative;
 *   this layer supplies the missing provider/apply/validate/repair execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/agent.h"
#include "umicom/ai_coding_runtime/task_queue.h"
#include "umicom/ai_coding_runtime/observation.h"
#include "umicom/base/snapshot_validation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiAiCodingAgent {
    int operation_active;
    UmiAiCodingAgentBindings bindings;
    UmiAiCodingRuntimeConfig config;
    UmiAiCodingValidationPlan validation_plan;
    UmiAiCodingChangeGuardPolicy change_guard;
    UmiAiCodingCancellation cancellation;
    UmiAiCodingEventQueue *events;
    UmiAiCodingHistory *history;
    UmiAiCodingAuditLog *audit;

    UmiAiCodingTaskPlan plan;
    UmiAiCodingMaterializedContext context;
    UmiAiCodingParsedResponse response;
    UmiAiCodingValidationReport validation;
    UmiAiCodingProviderExecution provider_execution;

    UmiAiCodingAgentSnapshot snapshot;
    char validation_feedback[UMI_AI_CODING_RUNTIME_TEXT_CAPACITY];
    uint64_t event_sequence;
    UmiAiCodingAgentPatchOutcome patch_outcome;
    int has_patch_outcome;
};

/* Activity belongs to the Framework execution owner. It is separate
 * from the visible task state, which can become terminal before an
 * outer call finishes. This is serialized reentry admission, not a lock. */
UmiStatus UmiAiCodingAgentCheckIdle(const UmiAiCodingAgent *agent)
{
    if (agent == NULL) return UMI_STATUS_OK;
    if (agent->operation_active) return UMI_STATUS_BUSY;
    return UmiAiCodingAssistantCheckIdle(agent->bindings.assistant);
}

static UmiStatus coding_agent_apply_internal(UmiAiCodingAgent *agent);

/* Provide the record snapshot operation used by this module and its client applications. */
/* State observations now publish their history, event and audit records through one admitted Framework operation; retain the former standalone history helper. The former implementation is retained for owner review. */
#if 0
static UmiStatus record_snapshot(UmiAiCodingAgent *agent)
{
    return umi_ai_coding_history_record(
        agent->history,
        &agent->snapshot);
}

#endif

/* Provide the emit operation used by this module and its client applications. */
/* Publish event and audit evidence together, advancing the producer sequence only after shared admission succeeds. The former implementation is retained for owner review. */
#if 0
static UmiStatus emit(
    UmiAiCodingAgent *agent,
    UmiAiCodingEventKind kind,
    UmiStatus status,
    const char *message)
{
    UmiAiCodingEvent event;

    (void)memset(&event, 0, sizeof(event));
    event.sequence = ++agent->event_sequence;
    event.kind = kind;
    event.state = agent->snapshot.state;
    event.status = status;
    (void)snprintf(
        event.task_id,
        sizeof(event.task_id),
        "%s",
        agent->snapshot.task_id);
    (void)snprintf(
        event.message,
        sizeof(event.message),
        "%s",
        message != NULL ? message : "");

    {
        UmiAiCodingAuditRecord audit;
        UmiStatus audit_status;

        (void)memset(&audit, 0, sizeof(audit));
        (void)snprintf(
            audit.task_id,
            sizeof(audit.task_id),
            "%s",
            agent->snapshot.task_id);
        audit.kind = kind;
        audit.state = agent->snapshot.state;
        audit.status = status;
        audit.plan_hash = agent->plan.plan_hash;
        audit.file_count = agent->response.has_patch
            ? agent->response.patch.file_count
            : 0U;
        audit.changed_lines = agent->response.has_patch
            ? agent->response.patch.changed_lines
            : 0U;
        (void)snprintf(
            audit.summary,
            sizeof(audit.summary),
            "%s",
            message != NULL ? message : "");

        audit_status = umi_ai_coding_audit_log_append(
            agent->audit,
            &audit);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (audit_status != UMI_STATUS_OK) return audit_status;
    }

    return umi_ai_coding_event_queue_push(agent->events, &event);
}

#endif

/* Event identity is committed only after all participating observation stores
 * accept the record. Refusal must not consume an identity or evict history. */
static UmiStatus publish_observation(UmiAiCodingAgent *agent,
    const UmiAiCodingAgentSnapshot *snapshot, int retain_snapshot,
    UmiAiCodingEventKind kind, UmiStatus status, const char *message)
{
    if (agent->event_sequence == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus checked = UmiAiCodingAgentSnapshotValidateText(snapshot);
    if (checked != UMI_STATUS_OK) return checked;
    UmiAiCodingEvent event = {0};
    UmiAiCodingAuditRecord audit = {0};
    event.sequence = agent->event_sequence + 1U;
    event.kind = kind; event.state = snapshot->state; event.status = status;
    (void)snprintf(event.task_id, sizeof(event.task_id), "%s", snapshot->task_id);
    (void)snprintf(event.message, sizeof(event.message), "%s", message != NULL ? message : "");
    audit.kind = kind; audit.state = snapshot->state; audit.status = status;
    audit.plan_hash = agent->plan.plan_hash;
    audit.file_count = agent->response.has_patch ? agent->response.patch.file_count : 0U;
    audit.changed_lines = agent->response.has_patch ? agent->response.patch.changed_lines : 0U;
    (void)snprintf(audit.task_id, sizeof(audit.task_id), "%s", snapshot->task_id);
    (void)snprintf(audit.summary, sizeof(audit.summary), "%s", message != NULL ? message : "");
    checked = UmiAiCodingObservationPublish(agent->events, agent->history, agent->audit,
        &event, retain_snapshot ? snapshot : NULL, &audit);
    if (checked == UMI_STATUS_OK) agent->event_sequence = event.sequence;
    return checked;
}

static UmiStatus emit(UmiAiCodingAgent *agent, UmiAiCodingEventKind kind,
    UmiStatus status, const char *message)
{
    return publish_observation(agent, &agent->snapshot, 0, kind, status, message);
}
/* Provide the set state operation used by this module and its client applications. */
/* Stage the state transition and its revision until event, audit and history admission succeed. This preserves the prior snapshot on observation refusal. The former implementation is retained for owner review. */
#if 0
static UmiStatus set_state(
    UmiAiCodingAgent *agent,
    UmiAiCodingRuntimeState state,
    UmiStatus status,
    const char *message)
{
    UmiStatus event_status;

    agent->snapshot.state = state;
    agent->snapshot.last_status = status;
    agent->snapshot.revision += 1U;

    event_status = record_snapshot(agent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (event_status != UMI_STATUS_OK) return event_status;

    return emit(agent, UMI_AI_CODING_EVENT_STATE, status, message);
}

#endif
/* Keep execution state authoritative when observation admission fails, so a full notification queue cannot conceal terminal failure. Only the related retained records are published together; the earlier staged execution-state draft remains for review. The former implementation is retained for owner review. */
#if 0

static UmiStatus set_state(UmiAiCodingAgent *agent, UmiAiCodingRuntimeState state,
    UmiStatus status, const char *message)
{
    if (agent->snapshot.revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiAiCodingAgentSnapshot candidate = agent->snapshot;
    candidate.state = state;
    candidate.last_status = status;
    ++candidate.revision;
    UmiStatus published = publish_observation(agent, &candidate, 1,
        UMI_AI_CODING_EVENT_STATE, status, message);
    if (published == UMI_STATUS_OK) agent->snapshot = candidate;
    return published;
}
#endif

/* Reserve the final state identity for termination and reject corrupt zero revisions. This replaces a guard that could spend the last identity on unfinished work; the former helper is retained for review. The former implementation is retained for owner review. */
#if 0
static UmiStatus set_state(UmiAiCodingAgent *agent, UmiAiCodingRuntimeState state,
    UmiStatus status, const char *message)
{
    if (agent->snapshot.revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiAiCodingAgentSnapshot candidate = agent->snapshot;
    candidate.state = state;
    candidate.last_status = status;
    ++candidate.revision;
    UmiStatus checked = UmiAiCodingAgentSnapshotValidateText(&candidate);
    if (checked != UMI_STATUS_OK) return checked;
    /* Execution state is authoritative even when its notification cannot be
     * retained. Otherwise a full queue can hide a terminal failure as BUSY.
     * Related history/audit/event records still publish together or not at all. */
    agent->snapshot = candidate;
    return publish_observation(agent, &candidate, 1,
        UMI_AI_CODING_EVENT_STATE, status, message);
}

#endif
/* Keep one identity available for a terminal transition. The final slot
 * cannot describe more work that would later need another state identity.
 * Notification backpressure still cannot roll authoritative state back. */
static UmiStatus set_state(UmiAiCodingAgent *agent, UmiAiCodingRuntimeState state,
    UmiStatus status, const char *message)
{
    if (agent->snapshot.revision == 0U) return UMI_STATUS_INVALID_STATE;
    if (agent->snapshot.revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    int terminal = state == UMI_AI_CODING_RUNTIME_IDLE ||
        state == UMI_AI_CODING_RUNTIME_COMPLETED ||
        state == UMI_AI_CODING_RUNTIME_FAILED ||
        state == UMI_AI_CODING_RUNTIME_CANCELLED;
    int exhausted = !terminal && agent->snapshot.revision == UINT64_MAX - 1U;
    UmiAiCodingAgentSnapshot candidate = agent->snapshot;
    candidate.state = exhausted ? UMI_AI_CODING_RUNTIME_FAILED : state;
    candidate.last_status = exhausted ? UMI_STATUS_CAPACITY_EXCEEDED : status;
    ++candidate.revision;
    UmiStatus checked = UmiAiCodingAgentSnapshotValidateText(&candidate);
    if (checked != UMI_STATUS_OK) return checked;
    /* Execution state is authoritative even when its notification cannot be
     * retained. Otherwise a full queue can hide a terminal failure as BUSY.
     * Related history/audit/event records still publish together or not at all. */
    agent->snapshot = candidate;
    UmiStatus published = publish_observation(agent, &candidate, 1,
        UMI_AI_CODING_EVENT_STATE, candidate.last_status,
        exhausted ? "Coding task stopped: state revision capacity exhausted." : message);
    return exhausted ? UMI_STATUS_CAPACITY_EXCEEDED : published;
}
/* Provide the cancelled operation used by this module and its client applications. */
static int cancelled(UmiAiCodingAgent *agent)
{
    return umi_ai_coding_cancellation_is_requested(&agent->cancellation);
}

/* Provide the check cancelled operation used by this module and its client applications. */
static UmiStatus check_cancelled(UmiAiCodingAgent *agent)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!cancelled(agent)) return UMI_STATUS_OK;

    agent->snapshot.cancelled = 1;
    (void)set_state(
        agent,
        UMI_AI_CODING_RUNTIME_CANCELLED,
        UMI_STATUS_CANCELLED,
        "Coding task cancelled.");
    return UMI_STATUS_CANCELLED;
}

/* Provide the prepare iteration operation used by this module and its client applications. */
static UmiStatus prepare_iteration(UmiAiCodingAgent *agent)
{
    UmiStatus status;

    status = set_state(
        agent,
        UMI_AI_CODING_RUNTIME_MATERIALISING_CONTEXT,
        UMI_STATUS_OK,
        "Materialising governed repository context.");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)memset(&agent->context, 0, sizeof(agent->context));

    status = umi_ai_coding_materialize_context(
        &agent->plan,
        &agent->bindings.workspace,
        &agent->context);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    agent->snapshot.context_files = agent->context.file_count;

    return emit(
        agent,
        UMI_AI_CODING_EVENT_CONTEXT,
        UMI_STATUS_OK,
        "Repository context materialised.");
}

/*
 * Provide the generate iteration operation used by this module and its client
 * applications.
 */
static UmiStatus generate_iteration(UmiAiCodingAgent *agent)
{
    UmiAiRequest provider_request;
    UmiAiResponse provider_response;
    char patch_id[UMI_AI_ID_CAPACITY];
    UmiStatus status;

    status = set_state(
        agent,
        agent->snapshot.iteration > 1U
            ? UMI_AI_CODING_RUNTIME_REPAIRING
            : UMI_AI_CODING_RUNTIME_GENERATING,
        UMI_STATUS_OK,
        agent->snapshot.iteration > 1U
            ? "Generating a repair from validation evidence."
            : "Generating coding response.");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_build_provider_request(
        &agent->plan,
        &agent->context,
        &agent->config,
        agent->validation_feedback[0] != '\0'
            ? agent->validation_feedback
            : NULL,
        &provider_request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_provider_execute(
        agent->bindings.ai_runtime,
        &agent->config,
        agent->plan.request.sensitive_approved,
        &provider_request,
        &provider_response,
        &agent->provider_execution);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)emit(
        agent,
        UMI_AI_CODING_EVENT_PROVIDER,
        UMI_STATUS_OK,
        "Provider response received.");

    (void)memset(&agent->response, 0, sizeof(agent->response));

    status = umi_ai_coding_parse_response(
        &provider_response,
        &agent->plan,
        &agent->bindings.workspace,
        &agent->response);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!agent->response.has_patch) return UMI_STATUS_OK;

    {
        UmiAiCodingChangeGuardResult guard;

        status = umi_ai_coding_change_guard_check(
            &agent->response.patch,
            &agent->change_guard,
            &guard);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        /* Apply this branch only when its contract condition is satisfied. */
        if (!guard.accepted) {
            (void)emit(
                agent,
                UMI_AI_CODING_EVENT_PATCH,
                UMI_STATUS_PERMISSION_DENIED,
                guard.reason);
            return UMI_STATUS_PERMISSION_DENIED;
        }
    }

    (void)snprintf(
        patch_id,
        sizeof(patch_id),
        "patch.%016llx.%u",
        (unsigned long long)agent->plan.plan_hash,
        agent->snapshot.iteration);
    (void)snprintf(
        agent->response.patch.patch_id,
        sizeof(agent->response.patch.patch_id),
        "%s",
        patch_id);

    status = umi_ai_coding_assistant_record_patch(
        agent->bindings.assistant,
        &agent->response.patch);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    agent->snapshot.patch_recorded = 1;
    agent->snapshot.patch_files = agent->response.patch.file_count;
    (void)snprintf(
        agent->snapshot.patch_id,
        sizeof(agent->snapshot.patch_id),
        "%s",
        agent->response.patch.patch_id);

    return emit(
        agent,
        UMI_AI_CODING_EVENT_PATCH,
        UMI_STATUS_OK,
        "Reviewable coding patch recorded.");
}

/*
 * Provide the approve pending internal operation used by this module and its client
 * applications.
 */
static UmiStatus approve_pending_internal(
    UmiAiCodingAgent *agent,
    const char *approved_by)
{
    UmiStatus status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!agent->snapshot.patch_recorded ||
        agent->snapshot.patch_id[0] == '\0') {
        return UMI_STATUS_NOT_FOUND;
    }

    status = umi_ai_coding_assistant_approve_patch(
        agent->bindings.assistant,
        agent->snapshot.patch_id,
        approved_by);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return emit(
        agent,
        UMI_AI_CODING_EVENT_PATCH,
        UMI_STATUS_OK,
        "Coding patch approved.");
}


/* Preserve detailed recovery evidence independently of the bounded event feed.
 * The assistant remains the only patch owner and recovery implementation. */
static UmiStatus execute_patch_transaction(UmiAiCodingAgent *agent,
    const UmiAiCodingFileAdapter *adapter, int reverting)
{
    UmiStatus status = UmiAiCodingAgentSnapshotValidateText(&agent->snapshot);
    if (status != UMI_STATUS_OK) return status;
    UmiAiCodingAgentPatchOutcome outcome = {0};
    (void)snprintf(outcome.task_id, sizeof(outcome.task_id), "%s", agent->snapshot.task_id);
    (void)snprintf(outcome.patch_id, sizeof(outcome.patch_id), "%s", agent->snapshot.patch_id);
    outcome.reverting = reverting;
    status = reverting ?
        UmiAiCodingAssistantRevertPatchWithReport(agent->bindings.assistant,
            outcome.patch_id, adapter, &outcome.transaction) :
        UmiAiCodingAssistantApplyPatchWithReport(agent->bindings.assistant,
            outcome.patch_id, adapter, &outcome.transaction);
    outcome.status = status;
    agent->patch_outcome = outcome;
    agent->has_patch_outcome = 1;
    return status;
}

/* Provide the apply patch operation used by this module and its client applications. */
static UmiStatus apply_patch(UmiAiCodingAgent *agent)
{
    UmiAiCodingFileAdapter file_adapter;
    UmiStatus status;

    status = umi_ai_coding_workspace_file_adapter(
        &agent->bindings.workspace,
        &file_adapter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = set_state(
        agent,
        UMI_AI_CODING_RUNTIME_APPLYING,
        UMI_STATUS_OK,
        "Applying approved coding patch.");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

/* Retain the complete assistant patch outcome for controller inspection after a failed mutation or notification. The former implementation is retained for owner review. */
#if 0
    status = umi_ai_coding_assistant_apply_patch(
        agent->bindings.assistant,
        agent->snapshot.patch_id,
        &file_adapter);
#endif
    status = execute_patch_transaction(agent, &file_adapter, 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    agent->snapshot.patch_applied = 1;

    return emit(
        agent,
        UMI_AI_CODING_EVENT_PATCH,
        UMI_STATUS_OK,
        "Coding patch applied.");
}

/* Provide the revert patch operation used by this module and its client applications. */
static UmiStatus revert_patch(UmiAiCodingAgent *agent)
{
    UmiAiCodingFileAdapter file_adapter;
    UmiStatus status;

    status = umi_ai_coding_workspace_file_adapter(
        &agent->bindings.workspace,
        &file_adapter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

/* Keep validation rollback recovery evidence available through the same owning service outcome. The former implementation is retained for owner review. */
#if 0
    status = umi_ai_coding_assistant_revert_patch(
        agent->bindings.assistant,
        agent->snapshot.patch_id,
        &file_adapter);
#endif
    status = execute_patch_transaction(agent, &file_adapter, 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    agent->snapshot.patch_applied = 0;
    agent->snapshot.rolled_back = 1;

    return emit(
        agent,
        UMI_AI_CODING_EVENT_ROLLBACK,
        UMI_STATUS_OK,
        "Validation failed; coding patch reverted.");
}


/* Provide the validate patch operation used by this module and its client applications. */
static UmiStatus validate_patch(UmiAiCodingAgent *agent)
{
    UmiStatus status;

    (void)memset(&agent->validation, 0, sizeof(agent->validation));

    /* Apply this operation only while the related capability or state is available. */
    if (agent->validation_plan.step_count == 0U) {
        /* Apply this operation only while the related capability or state is available. */
        if (agent->config.require_validation) {
            return UMI_STATUS_INVALID_STATE;
        }

        agent->snapshot.validation_passed = 1;
        return UMI_STATUS_OK;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent->bindings.executor == NULL ||
        agent->bindings.executor->execute == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = set_state(
        agent,
        UMI_AI_CODING_RUNTIME_VALIDATING,
        UMI_STATUS_OK,
        "Running configured build/test validation.");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_validation_run(
        &agent->validation_plan,
        agent->bindings.executor,
        &agent->validation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    agent->snapshot.validation_steps = agent->validation.result_count;
    agent->snapshot.validation_failures = agent->validation.failed_count;
    agent->snapshot.validation_passed = agent->validation.passed;

    (void)emit(
        agent,
        UMI_AI_CODING_EVENT_VALIDATION,
        agent->validation.passed
            ? UMI_STATUS_OK
            : UMI_STATUS_UNAVAILABLE,
        agent->validation.passed
            ? "Coding patch passed validation."
            : "Coding patch failed validation.");

    return UMI_STATUS_OK;
}

/* Provide the finish success operation used by this module and its client applications. */
static UmiStatus finish_success(UmiAiCodingAgent *agent)
{
    agent->snapshot.validation_passed =
        agent->snapshot.patch_applied
            ? agent->snapshot.validation_passed
            : 1;

    return set_state(
        agent,
        UMI_AI_CODING_RUNTIME_COMPLETED,
        UMI_STATUS_OK,
        "Coding task completed.");
}

/*
 * Provide the run after generation operation used by this module and its client
 * applications.
 */
/* Automatic apply is part of the already admitted outer execution. Use its private path while public reentry remains refused. The former implementation is retained for owner review. */
#if 0
static UmiStatus run_after_generation(UmiAiCodingAgent *agent)
{
    UmiStatus status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!agent->response.has_patch) {
        return finish_success(agent);
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!agent->config.auto_approve) {
        return set_state(
            agent,
            UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED,
            UMI_STATUS_OK,
            "Coding patch requires review and approval.");
    }

    status = approve_pending_internal(
        agent,
        "umicom-agent-policy");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!agent->config.auto_apply_approved_patch) {
        return set_state(
            agent,
            UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED,
            UMI_STATUS_OK,
            "Coding patch approved and waiting for explicit apply.");
    }

    return umi_ai_coding_agent_apply_pending(agent);
}

#endif
static UmiStatus run_after_generation(UmiAiCodingAgent *agent)
{
    UmiStatus status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!agent->response.has_patch) {
        return finish_success(agent);
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (!agent->config.auto_approve) {
        return set_state(
            agent,
            UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED,
            UMI_STATUS_OK,
            "Coding patch requires review and approval.");
    }

    status = approve_pending_internal(
        agent,
        "umicom-agent-policy");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!agent->config.auto_apply_approved_patch) {
        return set_state(
            agent,
            UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED,
            UMI_STATUS_OK,
            "Coding patch approved and waiting for explicit apply.");
    }

    return coding_agent_apply_internal(agent);
}

/*
 * Initialise ai coding agent from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_coding_agent_create(
    const UmiAiCodingAgentBindings *bindings,
    const UmiAiCodingRuntimeConfig *config,
    UmiAiCodingAgent **out_agent)
{
    UmiAiCodingAgent *agent;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL || config == NULL || out_agent == NULL ||
        bindings->ai_runtime == NULL ||
        bindings->assistant == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_coding_workspace_adapter_validate(&bindings->workspace);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_runtime_config_validate(config);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    *out_agent = NULL;

    agent = (UmiAiCodingAgent *)calloc(1U, sizeof(*agent));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    agent->bindings = *bindings;
    agent->config = *config;
    umi_ai_coding_validation_plan_init(&agent->validation_plan);
    umi_ai_coding_change_guard_policy_init(&agent->change_guard);
    agent->change_guard.maximum_files = config->maximum_context_files;
    umi_ai_coding_cancellation_init(&agent->cancellation);

    status = umi_ai_coding_event_queue_create(&agent->events);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_history_create(&agent->history);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_audit_log_create(&agent->audit);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_ai_coding_audit_log_destroy(agent->audit);
        umi_ai_coding_history_destroy(agent->history);
        umi_ai_coding_event_queue_destroy(agent->events);
        free(agent);
        return status;
    }

    agent->snapshot.state = UMI_AI_CODING_RUNTIME_IDLE;
    agent->snapshot.last_status = UMI_STATUS_OK;
    agent->snapshot.maximum_iterations = config->maximum_iterations;
    agent->snapshot.revision = 1U;

    *out_agent = agent;
    return UMI_STATUS_OK;
}

/* Release or reset state held by ai coding agent so the same storage can be reused safely. */
/* Retain the agent and its audit/event storage while callbacks or a borrowed assistant patch operation are active. The former implementation is retained for owner review. */
#if 0
void umi_ai_coding_agent_destroy(UmiAiCodingAgent *agent)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL) return;

    umi_ai_coding_audit_log_destroy(agent->audit);
    umi_ai_coding_history_destroy(agent->history);
    umi_ai_coding_event_queue_destroy(agent->events);
    free(agent);
}

#endif
UmiStatus UmiAiCodingAgentDestroyChecked(UmiAiCodingAgent *agent)
{
    UmiStatus status = UmiAiCodingAgentCheckIdle(agent);
    if (status != UMI_STATUS_OK || agent == NULL) return status;
    umi_ai_coding_audit_log_destroy(agent->audit);
    umi_ai_coding_history_destroy(agent->history);
    umi_ai_coding_event_queue_destroy(agent->events);
    free(agent); return UMI_STATUS_OK;
}

void umi_ai_coding_agent_destroy(UmiAiCodingAgent *agent)
{
    (void)UmiAiCodingAgentDestroyChecked(agent);
}

/*
 * Provide the ai coding agent set validation plan operation used by this module and its
 * client applications.
 */
/* Refuse callback-driven mutation before inspecting or changing the live execution state. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_agent_set_validation_plan(
    UmiAiCodingAgent *agent,
    const UmiAiCodingValidationPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL || plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (agent->snapshot.state != UMI_AI_CODING_RUNTIME_IDLE &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_COMPLETED &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_FAILED &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_CANCELLED) {
        return UMI_STATUS_BUSY;
    }

    agent->validation_plan = *plan;
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_coding_agent_set_validation_plan(
    UmiAiCodingAgent *agent,
    const UmiAiCodingValidationPlan *plan)
{
    if (agent != NULL && UmiAiCodingAgentCheckIdle(agent) != UMI_STATUS_OK)
        return UMI_STATUS_BUSY;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL || plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (agent->snapshot.state != UMI_AI_CODING_RUNTIME_IDLE &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_COMPLETED &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_FAILED &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_CANCELLED) {
        return UMI_STATUS_BUSY;
    }

    agent->validation_plan = *plan;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai coding agent set change guard operation used by this module and its
 * client applications.
 */
/* Refuse callback-driven mutation before inspecting or changing the live execution state. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_agent_set_change_guard(
    UmiAiCodingAgent *agent,
    const UmiAiCodingChangeGuardPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL || policy == NULL ||
        policy->maximum_files == 0U ||
        policy->maximum_changed_lines == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (agent->snapshot.state != UMI_AI_CODING_RUNTIME_IDLE &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_COMPLETED &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_FAILED &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_CANCELLED) {
        return UMI_STATUS_BUSY;
    }

    agent->change_guard = *policy;
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_coding_agent_set_change_guard(
    UmiAiCodingAgent *agent,
    const UmiAiCodingChangeGuardPolicy *policy)
{
    if (agent != NULL && UmiAiCodingAgentCheckIdle(agent) != UMI_STATUS_OK)
        return UMI_STATUS_BUSY;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL || policy == NULL ||
        policy->maximum_files == 0U ||
        policy->maximum_changed_lines == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (agent->snapshot.state != UMI_AI_CODING_RUNTIME_IDLE &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_COMPLETED &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_FAILED &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_CANCELLED) {
        return UMI_STATUS_BUSY;
    }

    agent->change_guard = *policy;
    return UMI_STATUS_OK;
}

/*
 * Perform ai coding agent through the module contract so client applications do not
 * duplicate its policy.
 */
/* Keep the serialized owner alive across all synchronous callbacks and clear activity on every return. Internal orchestration retains its existing behavior. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_agent_run(
    UmiAiCodingAgent *agent,
    const char *task_id,
    const UmiAiCodingRequest *request)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL || task_id == NULL || task_id[0] == '\0' ||
        request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (agent->snapshot.state != UMI_AI_CODING_RUNTIME_IDLE &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_COMPLETED &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_FAILED &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_CANCELLED) {
        return UMI_STATUS_BUSY;
    }

    umi_ai_coding_cancellation_init(&agent->cancellation);
    (void)memset(&agent->plan, 0, sizeof(agent->plan));
    (void)memset(&agent->context, 0, sizeof(agent->context));
    (void)memset(&agent->response, 0, sizeof(agent->response));
    (void)memset(&agent->validation, 0, sizeof(agent->validation));
    (void)memset(
        &agent->provider_execution,
        0,
        sizeof(agent->provider_execution));
    agent->validation_feedback[0] = '\0';

    (void)memset(&agent->snapshot, 0, sizeof(agent->snapshot));
    (void)snprintf(
        agent->snapshot.task_id,
        sizeof(agent->snapshot.task_id),
        "%s",
        task_id);
    (void)snprintf(
        agent->snapshot.request_id,
        sizeof(agent->snapshot.request_id),
        "%s",
        request->request_id);
    (void)snprintf(
        agent->snapshot.provider_id,
        sizeof(agent->snapshot.provider_id),
        "%s",
        agent->config.provider_id);
    (void)snprintf(
        agent->snapshot.model_id,
        sizeof(agent->snapshot.model_id),
        "%s",
        agent->config.model_id);
    agent->snapshot.maximum_iterations = agent->config.maximum_iterations;
    agent->snapshot.revision = 1U;

    status = set_state(
        agent,
        UMI_AI_CODING_RUNTIME_PREPARING,
        UMI_STATUS_OK,
        "Preparing governed coding task.");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    {
        UmiAiCodingRequest effective_request = *request;

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (effective_request.context_token_budget == 0U ||
            effective_request.context_token_budget >
                agent->config.context_token_budget) {
            effective_request.context_token_budget =
                agent->config.context_token_budget;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (effective_request.maximum_context_files == 0U ||
            effective_request.maximum_context_files >
                agent->config.maximum_context_files) {
            effective_request.maximum_context_files =
                agent->config.maximum_context_files;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (!agent->config.allow_sensitive_context) {
            effective_request.sensitive_approved = 0;
        }

        status = umi_ai_coding_assistant_prepare(
            agent->bindings.assistant,
            &effective_request,
            agent->config.provider_kind,
            &agent->plan);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    /* Visit each bounded item once so every record receives the same rule. */
    for (agent->snapshot.iteration = 1U;
         agent->snapshot.iteration <= agent->config.maximum_iterations;
         ++agent->snapshot.iteration) {
        status = check_cancelled(agent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        status = prepare_iteration(agent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto failure;

        status = generate_iteration(agent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto failure;

        status = run_after_generation(agent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto failure;

        /* Apply this branch only when its contract condition is satisfied. */
        if (agent->snapshot.state ==
            UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED) {
            return UMI_STATUS_OK;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (agent->snapshot.state ==
            UMI_AI_CODING_RUNTIME_COMPLETED) {
            return UMI_STATUS_OK;
        }

        /*
         * apply_pending() performs validation. A failed required validation
         * may roll back and leave state REPAIRING so this loop can use the
         * exact failure output as the next provider iteration.
         */
        if (agent->snapshot.validation_passed) {
            return finish_success(agent);
        }

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (agent->snapshot.iteration >=
            agent->config.maximum_iterations) {
            break;
        }
    }

    status = UMI_STATUS_UNAVAILABLE;

failure:
    (void)set_state(
        agent,
        UMI_AI_CODING_RUNTIME_FAILED,
        status,
        "Coding task failed.");
    return status;
}

#endif
/* Keep transition revisions monotonic across tasks and admit minimum terminal headroom before resetting task data. The former per-task reset is retained for owner review. The former implementation is retained for owner review. */
#if 0
static UmiStatus coding_agent_run_internal(
    UmiAiCodingAgent *agent,
    const char *task_id,
    const UmiAiCodingRequest *request)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL || task_id == NULL || task_id[0] == '\0' ||
        request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (agent->snapshot.state != UMI_AI_CODING_RUNTIME_IDLE &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_COMPLETED &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_FAILED &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_CANCELLED) {
        return UMI_STATUS_BUSY;
    }

    /* Prior task recovery must not be presented as this newly admitted task. */
    agent->has_patch_outcome = 0;
    memset(&agent->patch_outcome, 0, sizeof(agent->patch_outcome));
    umi_ai_coding_cancellation_init(&agent->cancellation);
    (void)memset(&agent->plan, 0, sizeof(agent->plan));
    (void)memset(&agent->context, 0, sizeof(agent->context));
    (void)memset(&agent->response, 0, sizeof(agent->response));
    (void)memset(&agent->validation, 0, sizeof(agent->validation));
    (void)memset(
        &agent->provider_execution,
        0,
        sizeof(agent->provider_execution));
    agent->validation_feedback[0] = '\0';

    (void)memset(&agent->snapshot, 0, sizeof(agent->snapshot));
    (void)snprintf(
        agent->snapshot.task_id,
        sizeof(agent->snapshot.task_id),
        "%s",
        task_id);
    (void)snprintf(
        agent->snapshot.request_id,
        sizeof(agent->snapshot.request_id),
        "%s",
        request->request_id);
    (void)snprintf(
        agent->snapshot.provider_id,
        sizeof(agent->snapshot.provider_id),
        "%s",
        agent->config.provider_id);
    (void)snprintf(
        agent->snapshot.model_id,
        sizeof(agent->snapshot.model_id),
        "%s",
        agent->config.model_id);
    agent->snapshot.maximum_iterations = agent->config.maximum_iterations;
    agent->snapshot.revision = 1U;

    status = set_state(
        agent,
        UMI_AI_CODING_RUNTIME_PREPARING,
        UMI_STATUS_OK,
        "Preparing governed coding task.");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
/* An initial notification failure still ends the admitted run in a terminal state; route it through the existing failure transition before returning the original status. The former implementation is retained for owner review. */
#if 0
    if (status != UMI_STATUS_OK) return status;
#endif
    if (status != UMI_STATUS_OK) goto failure;

    {
        UmiAiCodingRequest effective_request = *request;

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (effective_request.context_token_budget == 0U ||
            effective_request.context_token_budget >
                agent->config.context_token_budget) {
            effective_request.context_token_budget =
                agent->config.context_token_budget;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (effective_request.maximum_context_files == 0U ||
            effective_request.maximum_context_files >
                agent->config.maximum_context_files) {
            effective_request.maximum_context_files =
                agent->config.maximum_context_files;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (!agent->config.allow_sensitive_context) {
            effective_request.sensitive_approved = 0;
        }

        status = umi_ai_coding_assistant_prepare(
            agent->bindings.assistant,
            &effective_request,
            agent->config.provider_kind,
            &agent->plan);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    /* Visit each bounded item once so every record receives the same rule. */
    for (agent->snapshot.iteration = 1U;
         agent->snapshot.iteration <= agent->config.maximum_iterations;
         ++agent->snapshot.iteration) {
        status = check_cancelled(agent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        status = prepare_iteration(agent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto failure;

        status = generate_iteration(agent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto failure;

        status = run_after_generation(agent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto failure;

        /* Apply this branch only when its contract condition is satisfied. */
        if (agent->snapshot.state ==
            UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED) {
            return UMI_STATUS_OK;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (agent->snapshot.state ==
            UMI_AI_CODING_RUNTIME_COMPLETED) {
            return UMI_STATUS_OK;
        }

        /*
         * apply_pending() performs validation. A failed required validation
         * may roll back and leave state REPAIRING so this loop can use the
         * exact failure output as the next provider iteration.
         */
        if (agent->snapshot.validation_passed) {
            return finish_success(agent);
        }

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (agent->snapshot.iteration >=
            agent->config.maximum_iterations) {
            break;
        }
    }

    status = UMI_STATUS_UNAVAILABLE;

failure:
    (void)set_state(
        agent,
        UMI_AI_CODING_RUNTIME_FAILED,
        status,
        "Coding task failed.");
    return status;
}

#endif
/* Construct bounded stable task input before clearing prior observations, rather than formatting unchecked caller text or truncating its identity. The previous reset path is retained for review. The former implementation is retained for owner review. */
#if 0
static UmiStatus coding_agent_run_internal(
    UmiAiCodingAgent *agent,
    const char *task_id,
    const UmiAiCodingRequest *request)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL || task_id == NULL || task_id[0] == '\0' ||
        request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (agent->snapshot.state != UMI_AI_CODING_RUNTIME_IDLE &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_COMPLETED &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_FAILED &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_CANCELLED) {
        return UMI_STATUS_BUSY;
    }

    /* A task needs a preparing transition and room to record its failure.
     * Refuse before clearing prior results, cancellation or patch evidence.
     * Revision identity belongs to this agent lifetime, not to one task. */
    if (agent->snapshot.revision == 0U) return UMI_STATUS_INVALID_STATE;
    if (agent->snapshot.revision >= UINT64_MAX - 1U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    uint64_t previous_revision = agent->snapshot.revision;

    /* Prior task recovery must not be presented as this newly admitted task. */
    agent->has_patch_outcome = 0;
    memset(&agent->patch_outcome, 0, sizeof(agent->patch_outcome));
    umi_ai_coding_cancellation_init(&agent->cancellation);
    (void)memset(&agent->plan, 0, sizeof(agent->plan));
    (void)memset(&agent->context, 0, sizeof(agent->context));
    (void)memset(&agent->response, 0, sizeof(agent->response));
    (void)memset(&agent->validation, 0, sizeof(agent->validation));
    (void)memset(
        &agent->provider_execution,
        0,
        sizeof(agent->provider_execution));
    agent->validation_feedback[0] = '\0';

    (void)memset(&agent->snapshot, 0, sizeof(agent->snapshot));
    (void)snprintf(
        agent->snapshot.task_id,
        sizeof(agent->snapshot.task_id),
        "%s",
        task_id);
    (void)snprintf(
        agent->snapshot.request_id,
        sizeof(agent->snapshot.request_id),
        "%s",
        request->request_id);
    (void)snprintf(
        agent->snapshot.provider_id,
        sizeof(agent->snapshot.provider_id),
        "%s",
        agent->config.provider_id);
    (void)snprintf(
        agent->snapshot.model_id,
        sizeof(agent->snapshot.model_id),
        "%s",
        agent->config.model_id);
    agent->snapshot.maximum_iterations = agent->config.maximum_iterations;
    agent->snapshot.revision = previous_revision;

    status = set_state(
        agent,
        UMI_AI_CODING_RUNTIME_PREPARING,
        UMI_STATUS_OK,
        "Preparing governed coding task.");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
/* An initial notification failure still ends the admitted run in a terminal state; route it through the existing failure transition before returning the original status. The former implementation is retained for owner review. */
#if 0
    if (status != UMI_STATUS_OK) return status;
#endif
    if (status != UMI_STATUS_OK) goto failure;

    {
        UmiAiCodingRequest effective_request = *request;

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (effective_request.context_token_budget == 0U ||
            effective_request.context_token_budget >
                agent->config.context_token_budget) {
            effective_request.context_token_budget =
                agent->config.context_token_budget;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (effective_request.maximum_context_files == 0U ||
            effective_request.maximum_context_files >
                agent->config.maximum_context_files) {
            effective_request.maximum_context_files =
                agent->config.maximum_context_files;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (!agent->config.allow_sensitive_context) {
            effective_request.sensitive_approved = 0;
        }

        status = umi_ai_coding_assistant_prepare(
            agent->bindings.assistant,
            &effective_request,
            agent->config.provider_kind,
            &agent->plan);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    /* Visit each bounded item once so every record receives the same rule. */
    for (agent->snapshot.iteration = 1U;
         agent->snapshot.iteration <= agent->config.maximum_iterations;
         ++agent->snapshot.iteration) {
        status = check_cancelled(agent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        status = prepare_iteration(agent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto failure;

        status = generate_iteration(agent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto failure;

        status = run_after_generation(agent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto failure;

        /* Apply this branch only when its contract condition is satisfied. */
        if (agent->snapshot.state ==
            UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED) {
            return UMI_STATUS_OK;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (agent->snapshot.state ==
            UMI_AI_CODING_RUNTIME_COMPLETED) {
            return UMI_STATUS_OK;
        }

        /*
         * apply_pending() performs validation. A failed required validation
         * may roll back and leave state REPAIRING so this loop can use the
         * exact failure output as the next provider iteration.
         */
        if (agent->snapshot.validation_passed) {
            return finish_success(agent);
        }

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (agent->snapshot.iteration >=
            agent->config.maximum_iterations) {
            break;
        }
    }

    status = UMI_STATUS_UNAVAILABLE;

failure:
    (void)set_state(
        agent,
        UMI_AI_CODING_RUNTIME_FAILED,
        status,
        "Coding task failed.");
    return status;
}

#endif
static UmiStatus coding_agent_run_internal(
    UmiAiCodingAgent *agent,
    const char *task_id,
    const UmiAiCodingRequest *request)
{
    UmiStatus status;
    UmiAiCodingQueuedTask input;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL || task_id == NULL || task_id[0] == '\0' ||
        request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (agent->snapshot.state != UMI_AI_CODING_RUNTIME_IDLE &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_COMPLETED &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_FAILED &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_CANCELLED) {
        return UMI_STATUS_BUSY;
    }

    /* Validate bounded fields and retain input bytes before clearing any owner
     * storage that a serialized caller may have supplied as its input. */
    status = UmiAiCodingQueuedTaskPrepare(task_id, request, &input);
    if (status != UMI_STATUS_OK) return status;

    /* A task needs a preparing transition and room to record its failure.
     * Refuse before clearing prior results, cancellation or patch evidence.
     * Revision identity belongs to this agent lifetime, not to one task. */
    if (agent->snapshot.revision == 0U) return UMI_STATUS_INVALID_STATE;
    if (agent->snapshot.revision >= UINT64_MAX - 1U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    uint64_t previous_revision = agent->snapshot.revision;

    /* Prior task recovery must not be presented as this newly admitted task. */
    agent->has_patch_outcome = 0;
    memset(&agent->patch_outcome, 0, sizeof(agent->patch_outcome));
    umi_ai_coding_cancellation_init(&agent->cancellation);
    (void)memset(&agent->plan, 0, sizeof(agent->plan));
    (void)memset(&agent->context, 0, sizeof(agent->context));
    (void)memset(&agent->response, 0, sizeof(agent->response));
    (void)memset(&agent->validation, 0, sizeof(agent->validation));
    (void)memset(
        &agent->provider_execution,
        0,
        sizeof(agent->provider_execution));
    agent->validation_feedback[0] = '\0';

    (void)memset(&agent->snapshot, 0, sizeof(agent->snapshot));
    (void)snprintf(
        agent->snapshot.task_id,
        sizeof(agent->snapshot.task_id),
        "%s",
        input.task_id);
    (void)snprintf(
        agent->snapshot.request_id,
        sizeof(agent->snapshot.request_id),
        "%s",
        input.request.request_id);
    (void)snprintf(
        agent->snapshot.provider_id,
        sizeof(agent->snapshot.provider_id),
        "%s",
        agent->config.provider_id);
    (void)snprintf(
        agent->snapshot.model_id,
        sizeof(agent->snapshot.model_id),
        "%s",
        agent->config.model_id);
    agent->snapshot.maximum_iterations = agent->config.maximum_iterations;
    agent->snapshot.revision = previous_revision;

    status = set_state(
        agent,
        UMI_AI_CODING_RUNTIME_PREPARING,
        UMI_STATUS_OK,
        "Preparing governed coding task.");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
/* An initial notification failure still ends the admitted run in a terminal state; route it through the existing failure transition before returning the original status. The former implementation is retained for owner review. */
#if 0
    if (status != UMI_STATUS_OK) return status;
#endif
    if (status != UMI_STATUS_OK) goto failure;

    {
        UmiAiCodingRequest effective_request = input.request;

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (effective_request.context_token_budget == 0U ||
            effective_request.context_token_budget >
                agent->config.context_token_budget) {
            effective_request.context_token_budget =
                agent->config.context_token_budget;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (effective_request.maximum_context_files == 0U ||
            effective_request.maximum_context_files >
                agent->config.maximum_context_files) {
            effective_request.maximum_context_files =
                agent->config.maximum_context_files;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (!agent->config.allow_sensitive_context) {
            effective_request.sensitive_approved = 0;
        }

        status = umi_ai_coding_assistant_prepare(
            agent->bindings.assistant,
            &effective_request,
            agent->config.provider_kind,
            &agent->plan);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    /* Visit each bounded item once so every record receives the same rule. */
    for (agent->snapshot.iteration = 1U;
         agent->snapshot.iteration <= agent->config.maximum_iterations;
         ++agent->snapshot.iteration) {
        status = check_cancelled(agent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        status = prepare_iteration(agent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto failure;

        status = generate_iteration(agent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto failure;

        status = run_after_generation(agent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto failure;

        /* Apply this branch only when its contract condition is satisfied. */
        if (agent->snapshot.state ==
            UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED) {
            return UMI_STATUS_OK;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (agent->snapshot.state ==
            UMI_AI_CODING_RUNTIME_COMPLETED) {
            return UMI_STATUS_OK;
        }

        /*
         * apply_pending() performs validation. A failed required validation
         * may roll back and leave state REPAIRING so this loop can use the
         * exact failure output as the next provider iteration.
         */
        if (agent->snapshot.validation_passed) {
            return finish_success(agent);
        }

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (agent->snapshot.iteration >=
            agent->config.maximum_iterations) {
            break;
        }
    }

    status = UMI_STATUS_UNAVAILABLE;

failure:
    (void)set_state(
        agent,
        UMI_AI_CODING_RUNTIME_FAILED,
        status,
        "Coding task failed.");
    return status;
}

UmiStatus umi_ai_coding_agent_run(
    UmiAiCodingAgent *agent,
    const char *task_id,
    const UmiAiCodingRequest *request)
{
    if (agent == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiCodingAgentCheckIdle(agent);
    if (status != UMI_STATUS_OK) return status;
    agent->operation_active = 1;
    status = coding_agent_run_internal(agent, task_id, request);
    agent->operation_active = 0;
    return status;
}

/*
 * Provide the ai coding agent approve pending operation used by this module and its client
 * applications.
 */
/* Refuse callback-driven mutation before inspecting or changing the live execution state. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_agent_approve_pending(
    UmiAiCodingAgent *agent,
    const char *approved_by)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL || approved_by == NULL ||
        approved_by[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (agent->snapshot.state !=
        UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED) {
        return UMI_STATUS_INVALID_STATE;
    }

    return approve_pending_internal(agent, approved_by);
}

#endif
UmiStatus umi_ai_coding_agent_approve_pending(
    UmiAiCodingAgent *agent,
    const char *approved_by)
{
    if (agent != NULL && UmiAiCodingAgentCheckIdle(agent) != UMI_STATUS_OK)
        return UMI_STATUS_BUSY;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL || approved_by == NULL ||
        approved_by[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (agent->snapshot.state !=
        UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED) {
        return UMI_STATUS_INVALID_STATE;
    }

    return approve_pending_internal(agent, approved_by);
}

/*
 * Provide the ai coding agent apply pending operation used by this module and its client
 * applications.
 */
/* Keep the serialized owner alive across all synchronous callbacks and clear activity on every return. Internal orchestration retains its existing behavior. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_agent_apply_pending(
    UmiAiCodingAgent *agent)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!agent->snapshot.patch_recorded ||
        agent->snapshot.patch_id[0] == '\0') {
        return UMI_STATUS_NOT_FOUND;
    }

    status = check_cancelled(agent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = apply_patch(agent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    status = validate_patch(agent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    /* Apply this operation only while the related capability or state is available. */
    if (agent->validation.passed ||
        agent->validation_plan.step_count == 0U) {
        return finish_success(agent);
    }

    /* Apply this operation only while the related capability or state is available. */
    if (agent->config.rollback_on_validation_failure) {
        status = revert_patch(agent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto failure;
    }

    agent->validation_feedback[0] = '\0';

    status = umi_ai_coding_validation_feedback(
        &agent->validation,
        agent->validation_feedback,
        sizeof(agent->validation_feedback));

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_CAPACITY_EXCEEDED) {
        status = UMI_STATUS_OK;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    /* Apply this branch only when its contract condition is satisfied. */
    if (agent->snapshot.iteration <
        agent->config.maximum_iterations) {
        return set_state(
            agent,
            UMI_AI_CODING_RUNTIME_REPAIRING,
            UMI_STATUS_UNAVAILABLE,
            "Validation failed; repair iteration is available.");
    }

    status = UMI_STATUS_UNAVAILABLE;

failure:
    (void)set_state(
        agent,
        UMI_AI_CODING_RUNTIME_FAILED,
        status,
        "Coding patch apply or validation failed.");
    return status;
}

#endif
static UmiStatus coding_agent_apply_internal(
    UmiAiCodingAgent *agent)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!agent->snapshot.patch_recorded ||
        agent->snapshot.patch_id[0] == '\0') {
        return UMI_STATUS_NOT_FOUND;
    }

    status = check_cancelled(agent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = apply_patch(agent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    status = validate_patch(agent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    /* Apply this operation only while the related capability or state is available. */
    if (agent->validation.passed ||
        agent->validation_plan.step_count == 0U) {
        return finish_success(agent);
    }

    /* Apply this operation only while the related capability or state is available. */
    if (agent->config.rollback_on_validation_failure) {
        status = revert_patch(agent);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) goto failure;
    }

    agent->validation_feedback[0] = '\0';

    status = umi_ai_coding_validation_feedback(
        &agent->validation,
        agent->validation_feedback,
        sizeof(agent->validation_feedback));

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_CAPACITY_EXCEEDED) {
        status = UMI_STATUS_OK;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    /* Apply this branch only when its contract condition is satisfied. */
    if (agent->snapshot.iteration <
        agent->config.maximum_iterations) {
        return set_state(
            agent,
            UMI_AI_CODING_RUNTIME_REPAIRING,
            UMI_STATUS_UNAVAILABLE,
            "Validation failed; repair iteration is available.");
    }

    status = UMI_STATUS_UNAVAILABLE;

failure:
    (void)set_state(
        agent,
        UMI_AI_CODING_RUNTIME_FAILED,
        status,
        "Coding patch apply or validation failed.");
    return status;
}

UmiStatus umi_ai_coding_agent_apply_pending(
    UmiAiCodingAgent *agent)
{
    if (agent == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiCodingAgentCheckIdle(agent);
    if (status != UMI_STATUS_OK) return status;
    agent->operation_active = 1;
    status = coding_agent_apply_internal(agent);
    agent->operation_active = 0;
    return status;
}

/*
 * Provide the ai coding agent continue repair operation used by this module and its client
 * applications.
 */
/* Keep the serialized owner alive across all synchronous callbacks and clear activity on every return. Internal orchestration retains its existing behavior. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_agent_continue_repair(
    UmiAiCodingAgent *agent)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Apply this branch only when its contract condition is satisfied. */
    if (agent->snapshot.state != UMI_AI_CODING_RUNTIME_REPAIRING ||
        agent->snapshot.iteration >= agent->config.maximum_iterations) {
        return UMI_STATUS_INVALID_STATE;
    }

    agent->snapshot.iteration += 1U;

    status = check_cancelled(agent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = prepare_iteration(agent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    status = generate_iteration(agent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    status = run_after_generation(agent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    return UMI_STATUS_OK;

failure:
    (void)set_state(
        agent,
        UMI_AI_CODING_RUNTIME_FAILED,
        status,
        "Coding repair iteration failed.");
    return status;
}

#endif
static UmiStatus coding_agent_repair_internal(
    UmiAiCodingAgent *agent)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Apply this branch only when its contract condition is satisfied. */
    if (agent->snapshot.state != UMI_AI_CODING_RUNTIME_REPAIRING ||
        agent->snapshot.iteration >= agent->config.maximum_iterations) {
        return UMI_STATUS_INVALID_STATE;
    }

    agent->snapshot.iteration += 1U;

    status = check_cancelled(agent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = prepare_iteration(agent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    status = generate_iteration(agent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    status = run_after_generation(agent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) goto failure;

    return UMI_STATUS_OK;

failure:
    (void)set_state(
        agent,
        UMI_AI_CODING_RUNTIME_FAILED,
        status,
        "Coding repair iteration failed.");
    return status;
}

UmiStatus umi_ai_coding_agent_continue_repair(
    UmiAiCodingAgent *agent)
{
    if (agent == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiCodingAgentCheckIdle(agent);
    if (status != UMI_STATUS_OK) return status;
    agent->operation_active = 1;
    status = coding_agent_repair_internal(agent);
    agent->operation_active = 0;
    return status;
}

/*
 * Provide the ai coding agent reject pending operation used by this module and its client
 * applications.
 */
/* Refuse callback-driven mutation before inspecting or changing the live execution state. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_agent_reject_pending(
    UmiAiCodingAgent *agent)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Apply this branch only when its contract condition is satisfied. */
    if (agent->snapshot.state !=
        UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_coding_assistant_reject_patch(
        agent->bindings.assistant,
        agent->snapshot.patch_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return set_state(
        agent,
        UMI_AI_CODING_RUNTIME_CANCELLED,
        UMI_STATUS_CANCELLED,
        "Coding patch rejected.");
}

#endif
UmiStatus umi_ai_coding_agent_reject_pending(
    UmiAiCodingAgent *agent)
{
    if (agent != NULL && UmiAiCodingAgentCheckIdle(agent) != UMI_STATUS_OK)
        return UMI_STATUS_BUSY;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Apply this branch only when its contract condition is satisfied. */
    if (agent->snapshot.state !=
        UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_coding_assistant_reject_patch(
        agent->bindings.assistant,
        agent->snapshot.patch_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return set_state(
        agent,
        UMI_AI_CODING_RUNTIME_CANCELLED,
        UMI_STATUS_CANCELLED,
        "Coding patch rejected.");
}

/*
 * Provide the ai coding agent cancel operation used by this module and its client
 * applications.
 */
void umi_ai_coding_agent_cancel(UmiAiCodingAgent *agent)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent != NULL) {
        umi_ai_coding_cancellation_request(&agent->cancellation);
    }
}

/*
 * Provide the ai coding agent next event operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_agent_next_event(
    UmiAiCodingAgent *agent,
    UmiAiCodingEvent *out_event)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ai_coding_event_queue_pop(agent->events, out_event);
}

/*
 * Return the number of records represented by ai coding agent event without changing their
 * state.
 */
size_t umi_ai_coding_agent_event_count(
    const UmiAiCodingAgent *agent)
{
    return agent != NULL
        ? umi_ai_coding_event_queue_count(agent->events)
        : 0U;
}

/*
 * Provide the ai coding agent snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_agent_snapshot(
    const UmiAiCodingAgent *agent,
    UmiAiCodingAgentSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (agent == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_snapshot = agent->snapshot;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai coding agent last response operation used by this module and its client
 * applications.
 */
const UmiAiCodingParsedResponse *umi_ai_coding_agent_last_response(
    const UmiAiCodingAgent *agent)
{
    return agent != NULL ? &agent->response : NULL;
}

/*
 * Provide the ai coding agent last validation operation used by this module and its client
 * applications.
 */
const UmiAiCodingValidationReport *umi_ai_coding_agent_last_validation(
    const UmiAiCodingAgent *agent)
{
    return agent != NULL ? &agent->validation : NULL;
}

/*
 * Provide the ai coding agent history operation used by this module and its client
 * applications.
 */
UmiAiCodingHistory *umi_ai_coding_agent_history(UmiAiCodingAgent *agent)
{
    return agent != NULL ? agent->history : NULL;
}

/*
 * Provide the ai coding agent audit operation used by this module and its client
 * applications.
 */
UmiAiCodingAuditLog *umi_ai_coding_agent_audit(UmiAiCodingAgent *agent)
{
    return agent != NULL ? agent->audit : NULL;
}


UmiStatus UmiAiCodingAgentReadPatchOutcome(const UmiAiCodingAgent *agent,
    UmiAiCodingAgentPatchOutcome *outOutcome)
{
    if (agent == NULL || outOutcome == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!agent->has_patch_outcome) return UMI_STATUS_NOT_FOUND;
    static const UmiSnapshotTextField fields[] = {
        {"task_id", offsetof(UmiAiCodingAgentPatchOutcome, task_id), sizeof(((UmiAiCodingAgentPatchOutcome *)0)->task_id), 0},
        {"patch_id", offsetof(UmiAiCodingAgentPatchOutcome, patch_id), sizeof(((UmiAiCodingAgentPatchOutcome *)0)->patch_id), 0}
    };
    UmiStatus status = UmiSnapshotValidateTextFields(&agent->patch_outcome,
        sizeof(agent->patch_outcome), fields, sizeof(fields) / sizeof(fields[0]), NULL);
    if (status != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
    *outOutcome = agent->patch_outcome;
    return UMI_STATUS_OK;
}
