/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/completion_orchestration.c
 *
 * PURPOSE:
 *   Implement completion provider aggregation, policy enforcement, bounded
 *   candidate collection, deterministic ranking, resolution and cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/completion_orchestration.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorCompletionOrchestration {
    UmiEditorCompletionProviderRegistry *provider_registry;
    UmiEditorCompletionCandidateCollection *candidates;
    UmiEditorCompletionQueryResult *result;
    UmiEditorCompletionSession *session;
    UmiEditorCompletionProviderReport *reports;
    size_t report_count;
    size_t report_capacity;
    size_t eligible_provider_count;
    size_t invoked_provider_count;
    size_t failed_provider_count;
    UmiEditorCompletionRequest request;
    UmiEditorCompletionPolicy policy;
    UmiEditorCompletionOrchestrationState state;
    uint64_t provider_registry_revision;
    uint64_t revision;
    int truncated;
    int incomplete;
};

typedef struct CompletionSinkContext {
    UmiEditorCompletionOrchestration *orchestration;
    UmiEditorCompletionProviderDescriptor descriptor;
    size_t emitted_count;
    int truncated;
} CompletionSinkContext;

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

/* Provide the validate policy operation used by this module and its client applications. */
static UmiStatus validate_policy(const UmiEditorCompletionPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL ||
        policy->struct_size != (uint32_t)sizeof(*policy) ||
        policy->api_version != UMI_EDITOR_COMPLETION_ORCHESTRATION_API_VERSION ||
        policy->maximum_providers == 0U ||
        policy->maximum_candidates == 0U ||
        policy->maximum_results == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Provide the reserve reports operation used by this module and its client applications. */
static UmiStatus reserve_reports(UmiEditorCompletionOrchestration *orchestration,
                                 size_t required)
{
    size_t capacity;
    UmiEditorCompletionProviderReport *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= orchestration->report_capacity) return UMI_STATUS_OK;
    capacity = orchestration->report_capacity > 0U
        ? orchestration->report_capacity
        : 8U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorCompletionProviderReport *)realloc(
        orchestration->reports, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    orchestration->reports = replacement;
    orchestration->report_capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the add report operation used by this module and its client applications. */
static UmiStatus add_report(
    UmiEditorCompletionOrchestration *orchestration,
    const UmiEditorCompletionProviderReport *report)
{
    UmiStatus status = reserve_reports(orchestration,
                                       orchestration->report_count + 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    orchestration->reports[orchestration->report_count++] = *report;
    return UMI_STATUS_OK;
}

/*
 * Provide the provider blocked by policy operation used by this module and its client
 * applications.
 */
static int provider_blocked_by_policy(
    const UmiEditorCompletionProviderDescriptor *descriptor,
    const UmiEditorCompletionPolicy *policy,
    const UmiEditorCompletionRequest *request)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor->source == UMI_EDITOR_COMPLETION_SOURCE_AI &&
        (!policy->allow_ai || !request->allow_ai)) {
        return 1;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor->requires_network &&
        (!policy->allow_remote || !request->allow_remote)) {
        return 1;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor->requires_workspace_trust &&
        (!policy->trusted_workspace || !request->trusted_workspace)) {
        return 1;
    }
    return 0;
}

/* Provide the collect candidate operation used by this module and its client applications. */
static UmiStatus collect_candidate(
    const UmiEditorCompletionCandidate *candidate,
    void *user_data)
{
    CompletionSinkContext *context = (CompletionSinkContext *)user_data;
    UmiEditorCompletionCandidate stored;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || candidate == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_completion_request_is_cancelled(
            &context->orchestration->request)) {
        return UMI_STATUS_CANCELLED;
    }
    {
        size_t maximum_candidates =
            context->orchestration->policy.maximum_candidates;
        /* Apply this branch only when its contract condition is satisfied. */
        if (context->orchestration->request.maximum_candidates <
            maximum_candidates) {
            maximum_candidates =
                context->orchestration->request.maximum_candidates;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_editor_completion_candidate_collection_count(
                context->orchestration->candidates) >= maximum_candidates) {
            context->truncated = 1;
            context->orchestration->truncated = 1;
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    stored = *candidate;
    stored.struct_size = (uint32_t)sizeof(stored);
    stored.api_version = UMI_EDITOR_COMPLETION_CANDIDATE_API_VERSION;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (stored.provider_id[0] == '\0') {
        copy_text(stored.provider_id,
                  sizeof(stored.provider_id),
                  context->descriptor.id);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (stored.item.document_id[0] == '\0') {
        copy_text(stored.item.document_id,
                  sizeof(stored.item.document_id),
                  context->orchestration->request.document_id);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (stored.request_id == 0U) {
        stored.request_id = context->orchestration->request.request_id;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (stored.document_revision == 0U) {
        stored.document_revision =
            context->orchestration->request.document_revision;
    }
    stored.provider_priority = context->descriptor.priority;
    /* Apply this branch only when its contract condition is satisfied. */
    if (stored.source == UMI_EDITOR_COMPLETION_SOURCE_OTHER) {
        stored.source = context->descriptor.source;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (context->descriptor.source == UMI_EDITOR_COMPLETION_SOURCE_AI) {
        stored.flags |= UMI_EDITOR_COMPLETION_CANDIDATE_AI;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (context->descriptor.requires_network) {
        stored.flags |= UMI_EDITOR_COMPLETION_CANDIDATE_REMOTE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (context->descriptor.requires_workspace_trust) {
        stored.flags |= UMI_EDITOR_COMPLETION_CANDIDATE_REQUIRES_TRUST;
    }
    status = umi_editor_completion_candidate_collection_upsert(
        context->orchestration->candidates, &stored);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++context->emitted_count;
    return status;
}

/*
 * Provide the editor completion policy default operation used by this module and its
 * client applications.
 */
UmiEditorCompletionPolicy umi_editor_completion_policy_default(void)
{
    UmiEditorCompletionPolicy policy;

    (void)memset(&policy, 0, sizeof(policy));
    policy.struct_size = (uint32_t)sizeof(policy);
    policy.api_version = UMI_EDITOR_COMPLETION_ORCHESTRATION_API_VERSION;
    policy.maximum_providers = 32U;
    policy.maximum_candidates = 1024U;
    policy.maximum_results = 100U;
    policy.allow_ai = 1;
    policy.allow_remote = 0;
    policy.trusted_workspace = 0;
    policy.continue_on_provider_error = 1;
    policy.resolve_before_accept = 0;
    return policy;
}

/*
 * Initialise editor completion orchestration from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_completion_orchestration_create(
    UmiEditorCompletionProviderRegistry *provider_registry,
    UmiEditorCompletionOrchestration **out_orchestration)
{
    UmiEditorCompletionOrchestration *orchestration;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider_registry == NULL || out_orchestration == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_orchestration = NULL;
    orchestration = (UmiEditorCompletionOrchestration *)calloc(
        1U, sizeof(*orchestration));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    orchestration->provider_registry = provider_registry;
    orchestration->policy = umi_editor_completion_policy_default();
    orchestration->state = UMI_EDITOR_COMPLETION_ORCHESTRATION_IDLE;
    orchestration->revision = 1U;
    status = umi_editor_completion_candidate_collection_create(
        &orchestration->candidates);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_completion_query_result_create(
            &orchestration->result);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_completion_session_create(&orchestration->session);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_editor_completion_orchestration_destroy(orchestration);
        return status;
    }
    *out_orchestration = orchestration;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor completion orchestration so the same storage can
 * be reused safely.
 */
void umi_editor_completion_orchestration_destroy(
    UmiEditorCompletionOrchestration *orchestration)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return;
    umi_editor_completion_session_destroy(orchestration->session);
    umi_editor_completion_query_result_destroy(orchestration->result);
    umi_editor_completion_candidate_collection_destroy(
        orchestration->candidates);
    free(orchestration->reports);
    orchestration->reports = NULL;
    free(orchestration);
}

/*
 * Provide the editor completion orchestration set policy operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_completion_orchestration_set_policy(
    UmiEditorCompletionOrchestration *orchestration,
    const UmiEditorCompletionPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL || validate_policy(policy) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    orchestration->policy = *policy;
    orchestration->policy.allow_ai = policy->allow_ai != 0;
    orchestration->policy.allow_remote = policy->allow_remote != 0;
    orchestration->policy.trusted_workspace =
        policy->trusted_workspace != 0;
    orchestration->policy.continue_on_provider_error =
        policy->continue_on_provider_error != 0;
    orchestration->policy.resolve_before_accept =
        policy->resolve_before_accept != 0;
    orchestration->revision = next_revision(orchestration->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor completion orchestration begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_completion_orchestration_begin(
    UmiEditorCompletionOrchestration *orchestration,
    const UmiEditorCompletionRequest *request,
    const UmiEditorCompletionRankingConfig *ranking)
{
    UmiEditorCompletionRankingConfig effective_ranking;
    const size_t provider_count = orchestration != NULL
        ? umi_editor_completion_provider_registry_count(
              orchestration->provider_registry)
        : 0U;
    size_t position;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL ||
        umi_editor_completion_request_validate(request) != UMI_STATUS_OK ||
        ranking == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    orchestration->request = *request;
    orchestration->state = UMI_EDITOR_COMPLETION_ORCHESTRATION_COLLECTING;
    orchestration->report_count = 0U;
    orchestration->eligible_provider_count = 0U;
    orchestration->invoked_provider_count = 0U;
    orchestration->failed_provider_count = 0U;
    orchestration->truncated = 0;
    orchestration->incomplete = 0;
    orchestration->provider_registry_revision =
        umi_editor_completion_provider_registry_revision(
            orchestration->provider_registry);
    (void)umi_editor_completion_candidate_collection_clear(
        orchestration->candidates);
    (void)umi_editor_completion_query_result_clear(orchestration->result);

    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < provider_count; ++position) {
        UmiEditorCompletionProviderDescriptor descriptor;
        UmiEditorCompletionProviderReport report;
        UmiEditorCompletionProviderResponse response;
        CompletionSinkContext context;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_editor_completion_request_is_cancelled(request)) {
            orchestration->state = UMI_EDITOR_COMPLETION_ORCHESTRATION_CANCELLED;
            orchestration->revision = next_revision(orchestration->revision);
            return UMI_STATUS_CANCELLED;
        }
        status = umi_editor_completion_provider_registry_at(
            orchestration->provider_registry, position, &descriptor);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!umi_editor_completion_provider_supports_request(
                &descriptor, request)) {
            continue;
        }
        ++orchestration->eligible_provider_count;
        (void)memset(&report, 0, sizeof(report));
        report.struct_size = (uint32_t)sizeof(report);
        report.api_version = UMI_EDITOR_COMPLETION_ORCHESTRATION_API_VERSION;
        copy_text(report.provider_id,
                  sizeof(report.provider_id),
                  descriptor.id);
        /* Apply this branch only when its contract condition is satisfied. */
        if (provider_blocked_by_policy(&descriptor,
                                       &orchestration->policy,
                                       request) ||
            orchestration->invoked_provider_count >=
                orchestration->policy.maximum_providers) {
            report.status = UMI_STATUS_PERMISSION_DENIED;
            report.skipped_by_policy = 1;
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (orchestration->invoked_provider_count >=
                orchestration->policy.maximum_providers) {
                report.truncated = 1;
                orchestration->truncated = 1;
            }
            status = add_report(orchestration, &report);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
            continue;
        }
        (void)memset(&context, 0, sizeof(context));
        context.orchestration = orchestration;
        context.descriptor = descriptor;
        (void)memset(&response, 0, sizeof(response));
        ++orchestration->invoked_provider_count;
        status = umi_editor_completion_provider_registry_invoke(
            orchestration->provider_registry,
            descriptor.id,
            request,
            collect_candidate,
            &context,
            &response);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_CAPACITY_EXCEEDED && context.truncated) {
            status = UMI_STATUS_OK;
        }
        report.status = status;
        report.emitted_candidate_count = context.emitted_count;
        report.provider_revision = response.provider_revision;
        report.incomplete = response.incomplete;
        report.truncated = context.truncated;
        /* Apply this branch only when its contract condition is satisfied. */
        if (response.incomplete) orchestration->incomplete = 1;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) ++orchestration->failed_provider_count;
        status = add_report(orchestration, &report);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (report.status != UMI_STATUS_OK &&
            !orchestration->policy.continue_on_provider_error) {
            orchestration->state = UMI_EDITOR_COMPLETION_ORCHESTRATION_FAILED;
            orchestration->revision = next_revision(orchestration->revision);
            return report.status;
        }
    }

    effective_ranking = *ranking;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (effective_ranking.maximum_results >
        orchestration->policy.maximum_results) {
        effective_ranking.maximum_results =
            orchestration->policy.maximum_results;
    }
    status = umi_editor_completion_query_execute(orchestration->result,
                                                  orchestration->candidates,
                                                  request,
                                                  &effective_ranking);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_CANCELLED) {
        orchestration->state = UMI_EDITOR_COMPLETION_ORCHESTRATION_CANCELLED;
        orchestration->revision = next_revision(orchestration->revision);
        return status;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        orchestration->state = UMI_EDITOR_COMPLETION_ORCHESTRATION_FAILED;
        orchestration->revision = next_revision(orchestration->revision);
        return status;
    }
    status = umi_editor_completion_session_begin(orchestration->session,
                                                  orchestration->result,
                                                  request);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        orchestration->state = UMI_EDITOR_COMPLETION_ORCHESTRATION_FAILED;
        orchestration->revision = next_revision(orchestration->revision);
        return status;
    }
    orchestration->state = UMI_EDITOR_COMPLETION_ORCHESTRATION_READY;
    orchestration->revision = next_revision(orchestration->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor completion orchestration cancel operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_completion_orchestration_cancel(
    UmiEditorCompletionOrchestration *orchestration)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (orchestration->request.request_id != 0U) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (position = 0U; position < orchestration->report_count; ++position) {
            const UmiEditorCompletionProviderReport *report =
                &orchestration->reports[position];
            /* Apply this branch only when its contract condition is satisfied. */
            if (!report->skipped_by_policy) {
                (void)umi_editor_completion_provider_registry_cancel(
                    orchestration->provider_registry,
                    report->provider_id,
                    orchestration->request.request_id);
            }
        }
    }
    (void)umi_editor_completion_session_cancel(orchestration->session);
    orchestration->state = UMI_EDITOR_COMPLETION_ORCHESTRATION_CANCELLED;
    orchestration->revision = next_revision(orchestration->revision);
    return UMI_STATUS_OK;
}

/*
 * Find editor completion orchestration resolve while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_completion_orchestration_resolve_selected(
    UmiEditorCompletionOrchestration *orchestration)
{
    UmiEditorCompletionCandidate candidate;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this operation only while the related capability or state is available. */
    if (orchestration->state != UMI_EDITOR_COMPLETION_ORCHESTRATION_READY) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_editor_completion_session_selected(orchestration->session,
                                                     &candidate);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_completion_provider_registry_resolve(
        orchestration->provider_registry,
        candidate.provider_id,
        &orchestration->request,
        &candidate);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    candidate.flags |= UMI_EDITOR_COMPLETION_CANDIDATE_RESOLVED;
    status = umi_editor_completion_session_replace_selected(
        orchestration->session, &candidate);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        orchestration->revision = next_revision(orchestration->revision);
    }
    return status;
}

/*
 * Find editor completion orchestration report while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_completion_orchestration_report_at(
    const UmiEditorCompletionOrchestration *orchestration,
    size_t position,
    UmiEditorCompletionProviderReport *out_report)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= orchestration->report_count) return UMI_STATUS_NOT_FOUND;
    *out_report = orchestration->reports[position];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor completion orchestration snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_completion_orchestration_snapshot(
    const UmiEditorCompletionOrchestration *orchestration,
    UmiEditorCompletionOrchestrationSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version =
        UMI_EDITOR_COMPLETION_ORCHESTRATION_API_VERSION;
    out_snapshot->state = orchestration->state;
    out_snapshot->eligible_provider_count =
        orchestration->eligible_provider_count;
    out_snapshot->invoked_provider_count = orchestration->invoked_provider_count;
    out_snapshot->failed_provider_count = orchestration->failed_provider_count;
    out_snapshot->collected_candidate_count =
        umi_editor_completion_candidate_collection_count(
            orchestration->candidates);
    out_snapshot->result_count =
        umi_editor_completion_query_result_count(orchestration->result);
    out_snapshot->request_id = orchestration->request.request_id;
    out_snapshot->document_revision = orchestration->request.document_revision;
    out_snapshot->provider_registry_revision =
        orchestration->provider_registry_revision;
    out_snapshot->revision = orchestration->revision;
    out_snapshot->truncated = orchestration->truncated;
    out_snapshot->incomplete = orchestration->incomplete;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor completion orchestration session operation used by this module and
 * its client applications.
 */
UmiEditorCompletionSession *umi_editor_completion_orchestration_session(
    UmiEditorCompletionOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->session : NULL;
}

/*
 * Provide the editor completion orchestration result operation used by this module and its
 * client applications.
 */
const UmiEditorCompletionQueryResult *
umi_editor_completion_orchestration_result(
    const UmiEditorCompletionOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->result : NULL;
}

/*
 * Provide the editor completion orchestration revision operation used by this module and
 * its client applications.
 */
uint64_t umi_editor_completion_orchestration_revision(
    const UmiEditorCompletionOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->revision : 0U;
}
