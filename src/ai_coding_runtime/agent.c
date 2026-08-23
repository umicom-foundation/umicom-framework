/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/agent.c
 *
 * PURPOSE:
 *   Implement the operational coding-agent state machine. Existing coding
 *   assistant planning, privacy and patch transactions remain authoritative;
 *   this layer supplies the missing provider/apply/validate/repair execution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/agent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiAiCodingAgent {
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
};

static UmiStatus record_snapshot(UmiAiCodingAgent *agent)
{
    return umi_ai_coding_history_record(
        agent->history,
        &agent->snapshot);
}

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
        if (audit_status != UMI_STATUS_OK) return audit_status;
    }

    return umi_ai_coding_event_queue_push(agent->events, &event);
}

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
    if (event_status != UMI_STATUS_OK) return event_status;

    return emit(agent, UMI_AI_CODING_EVENT_STATE, status, message);
}

static int cancelled(UmiAiCodingAgent *agent)
{
    return umi_ai_coding_cancellation_is_requested(&agent->cancellation);
}

static UmiStatus check_cancelled(UmiAiCodingAgent *agent)
{
    if (!cancelled(agent)) return UMI_STATUS_OK;

    agent->snapshot.cancelled = 1;
    (void)set_state(
        agent,
        UMI_AI_CODING_RUNTIME_CANCELLED,
        UMI_STATUS_CANCELLED,
        "Coding task cancelled.");
    return UMI_STATUS_CANCELLED;
}

static UmiStatus prepare_iteration(UmiAiCodingAgent *agent)
{
    UmiStatus status;

    status = set_state(
        agent,
        UMI_AI_CODING_RUNTIME_MATERIALISING_CONTEXT,
        UMI_STATUS_OK,
        "Materialising governed repository context.");
    if (status != UMI_STATUS_OK) return status;

    (void)memset(&agent->context, 0, sizeof(agent->context));

    status = umi_ai_coding_materialize_context(
        &agent->plan,
        &agent->bindings.workspace,
        &agent->context);
    if (status != UMI_STATUS_OK) return status;

    agent->snapshot.context_files = agent->context.file_count;

    return emit(
        agent,
        UMI_AI_CODING_EVENT_CONTEXT,
        UMI_STATUS_OK,
        "Repository context materialised.");
}

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
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_build_provider_request(
        &agent->plan,
        &agent->context,
        &agent->config,
        agent->validation_feedback[0] != '\0'
            ? agent->validation_feedback
            : NULL,
        &provider_request);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_provider_execute(
        agent->bindings.ai_runtime,
        &agent->config,
        agent->plan.request.sensitive_approved,
        &provider_request,
        &provider_response,
        &agent->provider_execution);
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
    if (status != UMI_STATUS_OK) return status;

    if (!agent->response.has_patch) return UMI_STATUS_OK;

    {
        UmiAiCodingChangeGuardResult guard;

        status = umi_ai_coding_change_guard_check(
            &agent->response.patch,
            &agent->change_guard,
            &guard);
        if (status != UMI_STATUS_OK) return status;

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

static UmiStatus approve_pending_internal(
    UmiAiCodingAgent *agent,
    const char *approved_by)
{
    UmiStatus status;

    if (!agent->snapshot.patch_recorded ||
        agent->snapshot.patch_id[0] == '\0') {
        return UMI_STATUS_NOT_FOUND;
    }

    status = umi_ai_coding_assistant_approve_patch(
        agent->bindings.assistant,
        agent->snapshot.patch_id,
        approved_by);
    if (status != UMI_STATUS_OK) return status;

    return emit(
        agent,
        UMI_AI_CODING_EVENT_PATCH,
        UMI_STATUS_OK,
        "Coding patch approved.");
}

static UmiStatus apply_patch(UmiAiCodingAgent *agent)
{
    UmiAiCodingFileAdapter file_adapter;
    UmiStatus status;

    status = umi_ai_coding_workspace_file_adapter(
        &agent->bindings.workspace,
        &file_adapter);
    if (status != UMI_STATUS_OK) return status;

    status = set_state(
        agent,
        UMI_AI_CODING_RUNTIME_APPLYING,
        UMI_STATUS_OK,
        "Applying approved coding patch.");
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_assistant_apply_patch(
        agent->bindings.assistant,
        agent->snapshot.patch_id,
        &file_adapter);
    if (status != UMI_STATUS_OK) return status;

    agent->snapshot.patch_applied = 1;

    return emit(
        agent,
        UMI_AI_CODING_EVENT_PATCH,
        UMI_STATUS_OK,
        "Coding patch applied.");
}

static UmiStatus revert_patch(UmiAiCodingAgent *agent)
{
    UmiAiCodingFileAdapter file_adapter;
    UmiStatus status;

    status = umi_ai_coding_workspace_file_adapter(
        &agent->bindings.workspace,
        &file_adapter);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_assistant_revert_patch(
        agent->bindings.assistant,
        agent->snapshot.patch_id,
        &file_adapter);
    if (status != UMI_STATUS_OK) return status;

    agent->snapshot.patch_applied = 0;
    agent->snapshot.rolled_back = 1;

    return emit(
        agent,
        UMI_AI_CODING_EVENT_ROLLBACK,
        UMI_STATUS_OK,
        "Validation failed; coding patch reverted.");
}


static UmiStatus validate_patch(UmiAiCodingAgent *agent)
{
    UmiStatus status;

    (void)memset(&agent->validation, 0, sizeof(agent->validation));

    if (agent->validation_plan.step_count == 0U) {
        if (agent->config.require_validation) {
            return UMI_STATUS_INVALID_STATE;
        }

        agent->snapshot.validation_passed = 1;
        return UMI_STATUS_OK;
    }

    if (agent->bindings.executor == NULL ||
        agent->bindings.executor->execute == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = set_state(
        agent,
        UMI_AI_CODING_RUNTIME_VALIDATING,
        UMI_STATUS_OK,
        "Running configured build/test validation.");
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_validation_run(
        &agent->validation_plan,
        agent->bindings.executor,
        &agent->validation);
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

static UmiStatus run_after_generation(UmiAiCodingAgent *agent)
{
    UmiStatus status;

    if (!agent->response.has_patch) {
        return finish_success(agent);
    }

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
    if (status != UMI_STATUS_OK) return status;

    if (!agent->config.auto_apply_approved_patch) {
        return set_state(
            agent,
            UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED,
            UMI_STATUS_OK,
            "Coding patch approved and waiting for explicit apply.");
    }

    return umi_ai_coding_agent_apply_pending(agent);
}

UmiStatus umi_ai_coding_agent_create(
    const UmiAiCodingAgentBindings *bindings,
    const UmiAiCodingRuntimeConfig *config,
    UmiAiCodingAgent **out_agent)
{
    UmiAiCodingAgent *agent;
    UmiStatus status;

    if (bindings == NULL || config == NULL || out_agent == NULL ||
        bindings->ai_runtime == NULL ||
        bindings->assistant == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_coding_workspace_adapter_validate(&bindings->workspace);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_runtime_config_validate(config);
    if (status != UMI_STATUS_OK) return status;

    *out_agent = NULL;

    agent = (UmiAiCodingAgent *)calloc(1U, sizeof(*agent));
    if (agent == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    agent->bindings = *bindings;
    agent->config = *config;
    umi_ai_coding_validation_plan_init(&agent->validation_plan);
    umi_ai_coding_change_guard_policy_init(&agent->change_guard);
    agent->change_guard.maximum_files = config->maximum_context_files;
    umi_ai_coding_cancellation_init(&agent->cancellation);

    status = umi_ai_coding_event_queue_create(&agent->events);
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_history_create(&agent->history);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_ai_coding_audit_log_create(&agent->audit);
    }

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

void umi_ai_coding_agent_destroy(UmiAiCodingAgent *agent)
{
    if (agent == NULL) return;

    umi_ai_coding_audit_log_destroy(agent->audit);
    umi_ai_coding_history_destroy(agent->history);
    umi_ai_coding_event_queue_destroy(agent->events);
    free(agent);
}

UmiStatus umi_ai_coding_agent_set_validation_plan(
    UmiAiCodingAgent *agent,
    const UmiAiCodingValidationPlan *plan)
{
    if (agent == NULL || plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (agent->snapshot.state != UMI_AI_CODING_RUNTIME_IDLE &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_COMPLETED &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_FAILED &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_CANCELLED) {
        return UMI_STATUS_BUSY;
    }

    agent->validation_plan = *plan;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_agent_set_change_guard(
    UmiAiCodingAgent *agent,
    const UmiAiCodingChangeGuardPolicy *policy)
{
    if (agent == NULL || policy == NULL ||
        policy->maximum_files == 0U ||
        policy->maximum_changed_lines == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (agent->snapshot.state != UMI_AI_CODING_RUNTIME_IDLE &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_COMPLETED &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_FAILED &&
        agent->snapshot.state != UMI_AI_CODING_RUNTIME_CANCELLED) {
        return UMI_STATUS_BUSY;
    }

    agent->change_guard = *policy;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_agent_run(
    UmiAiCodingAgent *agent,
    const char *task_id,
    const UmiAiCodingRequest *request)
{
    UmiStatus status;

    if (agent == NULL || task_id == NULL || task_id[0] == '\0' ||
        request == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

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
    if (status != UMI_STATUS_OK) return status;

    {
        UmiAiCodingRequest effective_request = *request;

        if (effective_request.context_token_budget == 0U ||
            effective_request.context_token_budget >
                agent->config.context_token_budget) {
            effective_request.context_token_budget =
                agent->config.context_token_budget;
        }

        if (effective_request.maximum_context_files == 0U ||
            effective_request.maximum_context_files >
                agent->config.maximum_context_files) {
            effective_request.maximum_context_files =
                agent->config.maximum_context_files;
        }

        if (!agent->config.allow_sensitive_context) {
            effective_request.sensitive_approved = 0;
        }

        status = umi_ai_coding_assistant_prepare(
            agent->bindings.assistant,
            &effective_request,
            agent->config.provider_kind,
            &agent->plan);
    }
    if (status != UMI_STATUS_OK) goto failure;

    for (agent->snapshot.iteration = 1U;
         agent->snapshot.iteration <= agent->config.maximum_iterations;
         ++agent->snapshot.iteration) {
        status = check_cancelled(agent);
        if (status != UMI_STATUS_OK) return status;

        status = prepare_iteration(agent);
        if (status != UMI_STATUS_OK) goto failure;

        status = generate_iteration(agent);
        if (status != UMI_STATUS_OK) goto failure;

        status = run_after_generation(agent);
        if (status != UMI_STATUS_OK) goto failure;

        if (agent->snapshot.state ==
            UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED) {
            return UMI_STATUS_OK;
        }

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

UmiStatus umi_ai_coding_agent_approve_pending(
    UmiAiCodingAgent *agent,
    const char *approved_by)
{
    if (agent == NULL || approved_by == NULL ||
        approved_by[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (agent->snapshot.state !=
        UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED) {
        return UMI_STATUS_INVALID_STATE;
    }

    return approve_pending_internal(agent, approved_by);
}

UmiStatus umi_ai_coding_agent_apply_pending(
    UmiAiCodingAgent *agent)
{
    UmiStatus status;

    if (agent == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    if (!agent->snapshot.patch_recorded ||
        agent->snapshot.patch_id[0] == '\0') {
        return UMI_STATUS_NOT_FOUND;
    }

    status = check_cancelled(agent);
    if (status != UMI_STATUS_OK) return status;

    status = apply_patch(agent);
    if (status != UMI_STATUS_OK) goto failure;

    status = validate_patch(agent);
    if (status != UMI_STATUS_OK) goto failure;

    if (agent->validation.passed ||
        agent->validation_plan.step_count == 0U) {
        return finish_success(agent);
    }

    if (agent->config.rollback_on_validation_failure) {
        status = revert_patch(agent);
        if (status != UMI_STATUS_OK) goto failure;
    }

    agent->validation_feedback[0] = '\0';

    status = umi_ai_coding_validation_feedback(
        &agent->validation,
        agent->validation_feedback,
        sizeof(agent->validation_feedback));

    if (status == UMI_STATUS_CAPACITY_EXCEEDED) {
        status = UMI_STATUS_OK;
    }
    if (status != UMI_STATUS_OK) goto failure;

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

UmiStatus umi_ai_coding_agent_continue_repair(
    UmiAiCodingAgent *agent)
{
    UmiStatus status;

    if (agent == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    if (agent->snapshot.state != UMI_AI_CODING_RUNTIME_REPAIRING ||
        agent->snapshot.iteration >= agent->config.maximum_iterations) {
        return UMI_STATUS_INVALID_STATE;
    }

    agent->snapshot.iteration += 1U;

    status = check_cancelled(agent);
    if (status != UMI_STATUS_OK) return status;

    status = prepare_iteration(agent);
    if (status != UMI_STATUS_OK) goto failure;

    status = generate_iteration(agent);
    if (status != UMI_STATUS_OK) goto failure;

    status = run_after_generation(agent);
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

UmiStatus umi_ai_coding_agent_reject_pending(
    UmiAiCodingAgent *agent)
{
    UmiStatus status;

    if (agent == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    if (agent->snapshot.state !=
        UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_coding_assistant_reject_patch(
        agent->bindings.assistant,
        agent->snapshot.patch_id);
    if (status != UMI_STATUS_OK) return status;

    return set_state(
        agent,
        UMI_AI_CODING_RUNTIME_CANCELLED,
        UMI_STATUS_CANCELLED,
        "Coding patch rejected.");
}

void umi_ai_coding_agent_cancel(UmiAiCodingAgent *agent)
{
    if (agent != NULL) {
        umi_ai_coding_cancellation_request(&agent->cancellation);
    }
}

UmiStatus umi_ai_coding_agent_next_event(
    UmiAiCodingAgent *agent,
    UmiAiCodingEvent *out_event)
{
    if (agent == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_ai_coding_event_queue_pop(agent->events, out_event);
}

size_t umi_ai_coding_agent_event_count(
    const UmiAiCodingAgent *agent)
{
    return agent != NULL
        ? umi_ai_coding_event_queue_count(agent->events)
        : 0U;
}

UmiStatus umi_ai_coding_agent_snapshot(
    const UmiAiCodingAgent *agent,
    UmiAiCodingAgentSnapshot *out_snapshot)
{
    if (agent == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_snapshot = agent->snapshot;
    return UMI_STATUS_OK;
}

const UmiAiCodingParsedResponse *umi_ai_coding_agent_last_response(
    const UmiAiCodingAgent *agent)
{
    return agent != NULL ? &agent->response : NULL;
}

const UmiAiCodingValidationReport *umi_ai_coding_agent_last_validation(
    const UmiAiCodingAgent *agent)
{
    return agent != NULL ? &agent->validation : NULL;
}

UmiAiCodingHistory *umi_ai_coding_agent_history(UmiAiCodingAgent *agent)
{
    return agent != NULL ? agent->history : NULL;
}

UmiAiCodingAuditLog *umi_ai_coding_agent_audit(UmiAiCodingAgent *agent)
{
    return agent != NULL ? agent->audit : NULL;
}
