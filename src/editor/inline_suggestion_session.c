/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/inline_suggestion_session.c
 *
 * PURPOSE:
 *   Implement provider aggregation, policy, deterministic ghost-text ordering,
 *   streaming updates, typed-through state and partial/full acceptance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/inline_suggestion_session.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct UmiEditorInlineSuggestionSession {
    UmiEditorInlineSuggestionProviderRegistry *provider_registry;
    UmiEditorInlineSuggestionCollection *collection;
    UmiEditorInlineSuggestion *suggestions;
    size_t count;
    size_t capacity;
    size_t selected_position;
    UmiEditorInlineSuggestionProviderReport *reports;
    size_t report_count;
    size_t report_capacity;
    size_t invoked_provider_count;
    size_t failed_provider_count;
    UmiEditorCompletionRequest request;
    UmiEditorInlineSuggestionPolicy policy;
    UmiEditorInlineSuggestionSessionState state;
    uint64_t provider_registry_revision;
    uint64_t revision;
    int truncated;
    int incomplete;
};

typedef struct InlineSinkContext {
    UmiEditorInlineSuggestionSession *session;
    UmiEditorInlineSuggestionProviderDescriptor descriptor;
    size_t emitted_count;
    int truncated;
} InlineSinkContext;

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
static UmiStatus validate_policy(
    const UmiEditorInlineSuggestionPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL ||
        policy->struct_size != (uint32_t)sizeof(*policy) ||
        policy->api_version != UMI_EDITOR_INLINE_SUGGESTION_SESSION_API_VERSION ||
        policy->maximum_providers == 0U ||
        policy->maximum_suggestions == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the reserve suggestions operation used by this module and its client
 * applications.
 */
static UmiStatus reserve_suggestions(UmiEditorInlineSuggestionSession *session,
                                     size_t required)
{
    size_t capacity;
    UmiEditorInlineSuggestion *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= session->capacity) return UMI_STATUS_OK;
    capacity = session->capacity > 0U ? session->capacity : 8U;
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
    replacement = (UmiEditorInlineSuggestion *)realloc(
        session->suggestions, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->suggestions = replacement;
    session->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the reserve reports operation used by this module and its client applications. */
static UmiStatus reserve_reports(UmiEditorInlineSuggestionSession *session,
                                 size_t required)
{
    size_t capacity;
    UmiEditorInlineSuggestionProviderReport *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= session->report_capacity) return UMI_STATUS_OK;
    capacity = session->report_capacity > 0U ? session->report_capacity : 8U;
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
    replacement = (UmiEditorInlineSuggestionProviderReport *)realloc(
        session->reports, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->reports = replacement;
    session->report_capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the add report operation used by this module and its client applications. */
static UmiStatus add_report(
    UmiEditorInlineSuggestionSession *session,
    const UmiEditorInlineSuggestionProviderReport *report)
{
    UmiStatus status = reserve_reports(session, session->report_count + 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    session->reports[session->report_count++] = *report;
    return UMI_STATUS_OK;
}

/*
 * Provide the provider blocked by policy operation used by this module and its client
 * applications.
 */
static int provider_blocked_by_policy(
    const UmiEditorInlineSuggestionProviderDescriptor *descriptor,
    const UmiEditorInlineSuggestionPolicy *policy,
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

/*
 * Provide the collect suggestion operation used by this module and its client
 * applications.
 */
static UmiStatus collect_suggestion(
    const UmiEditorInlineSuggestion *suggestion,
    void *user_data)
{
    InlineSinkContext *context = (InlineSinkContext *)user_data;
    UmiEditorInlineSuggestion stored;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || suggestion == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_completion_request_is_cancelled(&context->session->request)) {
        return UMI_STATUS_CANCELLED;
    }
    {
        size_t maximum_suggestions =
            context->session->policy.maximum_suggestions;
        /* Apply this branch only when its contract condition is satisfied. */
        if (context->session->request.maximum_candidates <
            maximum_suggestions) {
            maximum_suggestions =
                context->session->request.maximum_candidates;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_editor_inline_suggestion_collection_count(
                context->session->collection) >= maximum_suggestions) {
            context->truncated = 1;
            context->session->truncated = 1;
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    stored = *suggestion;
    stored.struct_size = (uint32_t)sizeof(stored);
    stored.api_version = UMI_EDITOR_INLINE_SUGGESTION_API_VERSION;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (stored.provider_id[0] == '\0') {
        copy_text(stored.provider_id,
                  sizeof(stored.provider_id),
                  context->descriptor.id);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (stored.document_id[0] == '\0') {
        copy_text(stored.document_id,
                  sizeof(stored.document_id),
                  context->session->request.document_id);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (stored.request_id == 0U) {
        stored.request_id = context->session->request.request_id;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (stored.document_revision == 0U) {
        stored.document_revision = context->session->request.document_revision;
    }
    stored.provider_priority = context->descriptor.priority;
    /* Apply this branch only when its contract condition is satisfied. */
    if (stored.source == UMI_EDITOR_COMPLETION_SOURCE_OTHER) {
        stored.source = context->descriptor.source;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (context->descriptor.source == UMI_EDITOR_COMPLETION_SOURCE_AI) {
        stored.flags |= UMI_EDITOR_INLINE_SUGGESTION_AI;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (context->descriptor.requires_network) {
        stored.flags |= UMI_EDITOR_INLINE_SUGGESTION_REMOTE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (context->descriptor.requires_workspace_trust) {
        stored.flags |= UMI_EDITOR_INLINE_SUGGESTION_REQUIRES_TRUST;
    }
    status = umi_editor_inline_suggestion_collection_upsert(
        context->session->collection, &stored);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++context->emitted_count;
    return status;
}

/*
 * Provide the suggestion compare operation used by this module and its client
 * applications.
 */
static int suggestion_compare(const void *left_pointer,
                              const void *right_pointer)
{
    const UmiEditorInlineSuggestion *left =
        (const UmiEditorInlineSuggestion *)left_pointer;
    const UmiEditorInlineSuggestion *right =
        (const UmiEditorInlineSuggestion *)right_pointer;
    int order;

    /* Apply this branch only when its contract condition is satisfied. */
    if (left->provider_priority > right->provider_priority) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->provider_priority < right->provider_priority) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->confidence_milli > right->confidence_milli) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->confidence_milli < right->confidence_milli) return 1;
    order = strcmp(left->provider_id, right->provider_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order;
    return strcmp(left->id, right->id);
}

/*
 * Provide the find session suggestion operation used by this module and its client
 * applications.
 */
static size_t find_session_suggestion(
    const UmiEditorInlineSuggestionSession *session,
    const char *provider_id,
    const char *suggestion_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || provider_id == NULL || suggestion_id == NULL) {
        return SIZE_MAX;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < session->count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(session->suggestions[position].provider_id, provider_id) == 0 &&
            strcmp(session->suggestions[position].id, suggestion_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

/* Provide the acceptance length operation used by this module and its client applications. */
static size_t acceptance_length(const char *text,
                                UmiEditorInlineSuggestionAcceptanceKind kind)
{
    const size_t length = strlen(text);
    size_t position = 0U;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (kind == UMI_EDITOR_INLINE_SUGGESTION_ACCEPT_ALL) return length;
    /* Apply this branch only when its contract condition is satisfied. */
    if (kind == UMI_EDITOR_INLINE_SUGGESTION_ACCEPT_LINE) {
        const char *newline = strchr(text, '\n');
        return newline != NULL ? (size_t)(newline - text) + 1U : length;
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (position < length &&
           isspace((int)(unsigned char)text[position]) != 0) {
        ++position;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= length) return length;
    /* Apply this branch only when its contract condition is satisfied. */
    if (isalnum((int)(unsigned char)text[position]) != 0 ||
        text[position] == '_') {
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (position < length &&
               (isalnum((int)(unsigned char)text[position]) != 0 ||
                text[position] == '_')) {
            ++position;
        }
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        ++position;
    }
    return position;
}

/*
 * Provide the editor inline suggestion policy default operation used by this module and
 * its client applications.
 */
UmiEditorInlineSuggestionPolicy umi_editor_inline_suggestion_policy_default(
    void)
{
    UmiEditorInlineSuggestionPolicy policy;

    (void)memset(&policy, 0, sizeof(policy));
    policy.struct_size = (uint32_t)sizeof(policy);
    policy.api_version = UMI_EDITOR_INLINE_SUGGESTION_SESSION_API_VERSION;
    policy.maximum_providers = 16U;
    policy.maximum_suggestions = 32U;
    policy.allow_ai = 1;
    policy.allow_remote = 0;
    policy.trusted_workspace = 0;
    policy.continue_on_provider_error = 1;
    return policy;
}

/*
 * Initialise editor inline suggestion session from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_inline_suggestion_session_create(
    UmiEditorInlineSuggestionProviderRegistry *provider_registry,
    UmiEditorInlineSuggestionSession **out_session)
{
    UmiEditorInlineSuggestionSession *session;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider_registry == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_session = NULL;
    session = (UmiEditorInlineSuggestionSession *)calloc(1U, sizeof(*session));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->provider_registry = provider_registry;
    session->policy = umi_editor_inline_suggestion_policy_default();
    session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_IDLE;
    session->revision = 1U;
    status = umi_editor_inline_suggestion_collection_create(&session->collection);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(session);
        return status;
    }
    *out_session = session;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor inline suggestion session so the same storage can
 * be reused safely.
 */
void umi_editor_inline_suggestion_session_destroy(
    UmiEditorInlineSuggestionSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    umi_editor_inline_suggestion_collection_destroy(session->collection);
    free(session->suggestions);
    session->suggestions = NULL;
    free(session->reports);
    session->reports = NULL;
    free(session);
}

/*
 * Provide the editor inline suggestion session set policy operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_set_policy(
    UmiEditorInlineSuggestionSession *session,
    const UmiEditorInlineSuggestionPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || validate_policy(policy) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    session->policy = *policy;
    session->policy.allow_ai = policy->allow_ai != 0;
    session->policy.allow_remote = policy->allow_remote != 0;
    session->policy.trusted_workspace = policy->trusted_workspace != 0;
    session->policy.continue_on_provider_error =
        policy->continue_on_provider_error != 0;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor inline suggestion session begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_begin(
    UmiEditorInlineSuggestionSession *session,
    const UmiEditorCompletionRequest *request)
{
    const size_t provider_count = session != NULL
        ? umi_editor_inline_suggestion_provider_registry_count(
              session->provider_registry)
        : 0U;
    size_t position;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL ||
        umi_editor_completion_request_validate(request) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    session->request = *request;
    session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_COLLECTING;
    session->report_count = 0U;
    session->invoked_provider_count = 0U;
    session->failed_provider_count = 0U;
    session->count = 0U;
    session->selected_position = 0U;
    session->truncated = 0;
    session->incomplete = 0;
    session->provider_registry_revision =
        umi_editor_inline_suggestion_provider_registry_revision(
            session->provider_registry);
    (void)umi_editor_inline_suggestion_collection_clear(session->collection);

    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < provider_count; ++position) {
        UmiEditorInlineSuggestionProviderDescriptor descriptor;
        UmiEditorInlineSuggestionProviderReport report;
        UmiEditorInlineSuggestionProviderResponse response;
        InlineSinkContext context;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_editor_completion_request_is_cancelled(request)) {
            session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_CANCELLED;
            session->revision = next_revision(session->revision);
            return UMI_STATUS_CANCELLED;
        }
        status = umi_editor_inline_suggestion_provider_registry_at(
            session->provider_registry, position, &descriptor);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!umi_editor_inline_suggestion_provider_supports_request(
                &descriptor, request)) {
            continue;
        }
        (void)memset(&report, 0, sizeof(report));
        report.struct_size = (uint32_t)sizeof(report);
        report.api_version = UMI_EDITOR_INLINE_SUGGESTION_SESSION_API_VERSION;
        copy_text(report.provider_id,
                  sizeof(report.provider_id),
                  descriptor.id);
        /* Apply this branch only when its contract condition is satisfied. */
        if (provider_blocked_by_policy(&descriptor,
                                       &session->policy,
                                       request) ||
            session->invoked_provider_count >= session->policy.maximum_providers) {
            report.status = UMI_STATUS_PERMISSION_DENIED;
            report.skipped_by_policy = 1;
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (session->invoked_provider_count >=
                session->policy.maximum_providers) {
                report.truncated = 1;
                session->truncated = 1;
            }
            status = add_report(session, &report);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
            continue;
        }
        (void)memset(&context, 0, sizeof(context));
        context.session = session;
        context.descriptor = descriptor;
        (void)memset(&response, 0, sizeof(response));
        ++session->invoked_provider_count;
        status = umi_editor_inline_suggestion_provider_registry_invoke(
            session->provider_registry,
            descriptor.id,
            request,
            collect_suggestion,
            &context,
            &response);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_CAPACITY_EXCEEDED && context.truncated) {
            status = UMI_STATUS_OK;
        }
        report.status = status;
        report.emitted_suggestion_count = context.emitted_count;
        report.provider_revision = response.provider_revision;
        report.incomplete = response.incomplete;
        report.streaming = response.streaming;
        report.truncated = context.truncated;
        /* Apply this branch only when its contract condition is satisfied. */
        if (response.incomplete) session->incomplete = 1;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) ++session->failed_provider_count;
        status = add_report(session, &report);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (report.status != UMI_STATUS_OK &&
            !session->policy.continue_on_provider_error) {
            session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_FAILED;
            session->revision = next_revision(session->revision);
            return report.status;
        }
    }

    session->count =
        umi_editor_inline_suggestion_collection_count(session->collection);
    status = reserve_suggestions(session, session->count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < session->count; ++position) {
        status = umi_editor_inline_suggestion_collection_at(
            session->collection, position, &session->suggestions[position]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (session->count > 1U) {
        qsort(session->suggestions,
              session->count,
              sizeof(*session->suggestions),
              suggestion_compare);
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (session->count == 0U) {
        session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_EMPTY;
    } else /* Apply this branch only when its contract condition is satisfied. */ if ((session->suggestions[0].flags &
                UMI_EDITOR_INLINE_SUGGESTION_STREAMING) != 0U) {
        session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_STREAMING;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_SHOWING;
    }
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor inline suggestion session cancel operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_cancel(
    UmiEditorInlineSuggestionSession *session)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < session->report_count; ++position) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (!session->reports[position].skipped_by_policy) {
            (void)umi_editor_inline_suggestion_provider_registry_cancel(
                session->provider_registry,
                session->reports[position].provider_id,
                session->request.request_id);
        }
    }
    session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_CANCELLED;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor inline suggestion session select next operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_select_next(
    UmiEditorInlineSuggestionSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((session->state != UMI_EDITOR_INLINE_SUGGESTION_SESSION_SHOWING &&
         session->state != UMI_EDITOR_INLINE_SUGGESTION_SESSION_STREAMING) ||
        session->count == 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    session->selected_position =
        (session->selected_position + 1U) % session->count;
    session->state =
        (session->suggestions[session->selected_position].flags &
         UMI_EDITOR_INLINE_SUGGESTION_STREAMING) != 0U
            ? UMI_EDITOR_INLINE_SUGGESTION_SESSION_STREAMING
            : UMI_EDITOR_INLINE_SUGGESTION_SESSION_SHOWING;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor inline suggestion session select previous operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_select_previous(
    UmiEditorInlineSuggestionSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((session->state != UMI_EDITOR_INLINE_SUGGESTION_SESSION_SHOWING &&
         session->state != UMI_EDITOR_INLINE_SUGGESTION_SESSION_STREAMING) ||
        session->count == 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    session->selected_position = session->selected_position == 0U
        ? session->count - 1U
        : session->selected_position - 1U;
    session->state =
        (session->suggestions[session->selected_position].flags &
         UMI_EDITOR_INLINE_SUGGESTION_STREAMING) != 0U
            ? UMI_EDITOR_INLINE_SUGGESTION_SESSION_STREAMING
            : UMI_EDITOR_INLINE_SUGGESTION_SESSION_SHOWING;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor inline suggestion session current operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_current(
    const UmiEditorInlineSuggestionSession *session,
    UmiEditorInlineSuggestion *out_suggestion)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_suggestion == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((session->state != UMI_EDITOR_INLINE_SUGGESTION_SESSION_SHOWING &&
         session->state != UMI_EDITOR_INLINE_SUGGESTION_SESSION_STREAMING) ||
        session->selected_position >= session->count) {
        return UMI_STATUS_INVALID_STATE;
    }
    *out_suggestion = session->suggestions[session->selected_position];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor inline suggestion session append chunk operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_append_chunk(
    UmiEditorInlineSuggestionSession *session,
    const char *provider_id,
    const char *suggestion_id,
    const char *chunk,
    int complete)
{
    size_t position;
    size_t existing_length;
    size_t chunk_length;
    UmiEditorInlineSuggestion *suggestion;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || provider_id == NULL || suggestion_id == NULL ||
        chunk == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_session_suggestion(session, provider_id, suggestion_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    suggestion = &session->suggestions[position];
    existing_length = strlen(suggestion->text);
    chunk_length = strlen(chunk);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (chunk_length >= sizeof(suggestion->text) - existing_length) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (chunk_length > 0U) {
        (void)memcpy(suggestion->text + existing_length, chunk, chunk_length);
    }
    suggestion->text[existing_length + chunk_length] = '\0';
    /* Apply this branch only when its contract condition is satisfied. */
    if (complete) {
        suggestion->flags &=
            ~(UmiEditorInlineSuggestionFlags)
                UMI_EDITOR_INLINE_SUGGESTION_STREAMING;
        suggestion->flags |= UMI_EDITOR_INLINE_SUGGESTION_COMPLETE;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        suggestion->flags |= UMI_EDITOR_INLINE_SUGGESTION_STREAMING;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (position == session->selected_position) {
        session->state = complete
            ? UMI_EDITOR_INLINE_SUGGESTION_SESSION_SHOWING
            : UMI_EDITOR_INLINE_SUGGESTION_SESSION_STREAMING;
    }
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor inline suggestion session typed through operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_typed_through(
    UmiEditorInlineSuggestionSession *session,
    const char *typed_text,
    uint64_t current_document_revision)
{
    UmiEditorInlineSuggestion *suggestion;
    size_t typed_length;
    size_t remaining_length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || typed_text == NULL || typed_text[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((session->state != UMI_EDITOR_INLINE_SUGGESTION_SESSION_SHOWING &&
         session->state != UMI_EDITOR_INLINE_SUGGESTION_SESSION_STREAMING) ||
        session->selected_position >= session->count) {
        return UMI_STATUS_INVALID_STATE;
    }
    suggestion = &session->suggestions[session->selected_position];
    /* Apply this branch only when its contract condition is satisfied. */
    if (suggestion->replace_start_offset != suggestion->replace_end_offset ||
        !umi_editor_inline_suggestion_accepts_typed_prefix(suggestion,
                                                           typed_text)) {
        session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_REJECTED;
        session->revision = next_revision(session->revision);
        return UMI_STATUS_INVALID_STATE;
    }
    typed_length = strlen(typed_text);
    remaining_length = strlen(suggestion->text) - typed_length;
    (void)memmove(suggestion->text,
                  suggestion->text + typed_length,
                  remaining_length + 1U);
    suggestion->replace_start_offset += typed_length;
    suggestion->replace_end_offset = suggestion->replace_start_offset;
    suggestion->document_revision = current_document_revision;
    session->request.document_revision = current_document_revision;
    /* Apply this branch only when its contract condition is satisfied. */
    if (remaining_length == 0U) {
        session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_ACCEPTED;
    }
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor inline suggestion session accept operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_accept(
    UmiEditorInlineSuggestionSession *session,
    UmiEditorTextBuffer *buffer,
    uint64_t current_document_revision,
    UmiEditorInlineSuggestionAcceptanceKind kind,
    UmiEditorInlineSuggestionAcceptance *out_acceptance)
{
    UmiEditorInlineSuggestionProviderDescriptor descriptor;
    UmiEditorInlineSuggestion *suggestion;
    UmiEditorTextBufferView view;
    size_t start;
    size_t end;
    size_t accepted_length;
    size_t full_length;
    size_t remaining_length;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || buffer == NULL || out_acceptance == NULL ||
        kind < UMI_EDITOR_INLINE_SUGGESTION_ACCEPT_ALL ||
        kind > UMI_EDITOR_INLINE_SUGGESTION_ACCEPT_LINE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((session->state != UMI_EDITOR_INLINE_SUGGESTION_SESSION_SHOWING &&
         session->state != UMI_EDITOR_INLINE_SUGGESTION_SESSION_STREAMING) ||
        session->selected_position >= session->count) {
        return UMI_STATUS_INVALID_STATE;
    }
    suggestion = &session->suggestions[session->selected_position];
    /* Apply this branch only when its contract condition is satisfied. */
    if (kind != UMI_EDITOR_INLINE_SUGGESTION_ACCEPT_ALL) {
        status = umi_editor_inline_suggestion_provider_registry_find(
            session->provider_registry,
            suggestion->provider_id,
            &descriptor);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!descriptor.supports_partial_acceptance) {
            return UMI_STATUS_NOT_IMPLEMENTED;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (suggestion->replace_start_offset !=
            suggestion->replace_end_offset) {
            return UMI_STATUS_INVALID_STATE;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((session->request.document_revision != 0U &&
         current_document_revision != session->request.document_revision) ||
        (suggestion->document_revision != 0U &&
         current_document_revision != suggestion->document_revision) ||
        suggestion->replace_start_offset > SIZE_MAX ||
        suggestion->replace_end_offset > SIZE_MAX) {
        session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_FAILED;
        session->revision = next_revision(session->revision);
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_editor_text_buffer_view(buffer, &view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (view.revision != current_document_revision) {
        session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_FAILED;
        session->revision = next_revision(session->revision);
        return UMI_STATUS_INVALID_STATE;
    }
    start = (size_t)suggestion->replace_start_offset;
    end = (size_t)suggestion->replace_end_offset;
    /* Apply this branch only when its contract condition is satisfied. */
    if (end < start || start > view.byte_count || end > view.byte_count) {
        session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_FAILED;
        session->revision = next_revision(session->revision);
        return UMI_STATUS_INVALID_STATE;
    }
    full_length = strlen(suggestion->text);
    accepted_length = acceptance_length(suggestion->text, kind);
    /* Apply this branch only when its contract condition is satisfied. */
    if (accepted_length == 0U || accepted_length > full_length) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_editor_text_buffer_replace(buffer,
                                            start,
                                            end - start,
                                            suggestion->text,
                                            accepted_length);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_FAILED;
        session->revision = next_revision(session->revision);
        return status;
    }
    (void)memset(out_acceptance, 0, sizeof(*out_acceptance));
    out_acceptance->struct_size = (uint32_t)sizeof(*out_acceptance);
    out_acceptance->api_version =
        UMI_EDITOR_INLINE_SUGGESTION_SESSION_API_VERSION;
    out_acceptance->suggestion = *suggestion;
    /* Apply this branch only when its contract condition is satisfied. */
    if (accepted_length > 0U) {
        (void)memcpy(out_acceptance->inserted_text,
                     suggestion->text,
                     accepted_length);
    }
    out_acceptance->inserted_text[accepted_length] = '\0';
    out_acceptance->inserted_byte_count = accepted_length;
    out_acceptance->kind = kind;
    out_acceptance->resulting_document_revision =
        umi_editor_text_buffer_revision(buffer);
    remaining_length = full_length - accepted_length;
    out_acceptance->remaining_byte_count = remaining_length;
    out_acceptance->complete = remaining_length == 0U;

    /* Apply this branch only when its contract condition is satisfied. */
    if (remaining_length == 0U) {
        suggestion->text[0] = '\0';
        session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_ACCEPTED;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        (void)memmove(suggestion->text,
                      suggestion->text + accepted_length,
                      remaining_length + 1U);
        suggestion->replace_start_offset = start + accepted_length;
        suggestion->replace_end_offset = suggestion->replace_start_offset;
        suggestion->document_revision = out_acceptance->resulting_document_revision;
        session->request.document_revision =
            out_acceptance->resulting_document_revision;
        session->state =
            (suggestion->flags & UMI_EDITOR_INLINE_SUGGESTION_STREAMING) != 0U
                ? UMI_EDITOR_INLINE_SUGGESTION_SESSION_STREAMING
                : UMI_EDITOR_INLINE_SUGGESTION_SESSION_SHOWING;
    }
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor inline suggestion session reject operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_reject(
    UmiEditorInlineSuggestionSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->state != UMI_EDITOR_INLINE_SUGGESTION_SESSION_SHOWING &&
        session->state != UMI_EDITOR_INLINE_SUGGESTION_SESSION_STREAMING) {
        return UMI_STATUS_INVALID_STATE;
    }
    session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_REJECTED;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Find editor inline suggestion session report while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_inline_suggestion_session_report_at(
    const UmiEditorInlineSuggestionSession *session,
    size_t position,
    UmiEditorInlineSuggestionProviderReport *out_report)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= session->report_count) return UMI_STATUS_NOT_FOUND;
    *out_report = session->reports[position];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor inline suggestion session snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_snapshot(
    const UmiEditorInlineSuggestionSession *session,
    UmiEditorInlineSuggestionSessionSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version =
        UMI_EDITOR_INLINE_SUGGESTION_SESSION_API_VERSION;
    out_snapshot->state = session->state;
    out_snapshot->suggestion_count = session->count;
    out_snapshot->selected_position = session->selected_position;
    out_snapshot->invoked_provider_count = session->invoked_provider_count;
    out_snapshot->failed_provider_count = session->failed_provider_count;
    out_snapshot->request_id = session->request.request_id;
    out_snapshot->document_revision = session->request.document_revision;
    out_snapshot->provider_registry_revision =
        session->provider_registry_revision;
    out_snapshot->revision = session->revision;
    out_snapshot->truncated = session->truncated;
    out_snapshot->incomplete = session->incomplete;
    out_snapshot->can_accept =
        (session->state == UMI_EDITOR_INLINE_SUGGESTION_SESSION_SHOWING ||
         session->state == UMI_EDITOR_INLINE_SUGGESTION_SESSION_STREAMING) &&
        session->selected_position < session->count;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor inline suggestion session revision operation used by this module and
 * its client applications.
 */
uint64_t umi_editor_inline_suggestion_session_revision(
    const UmiEditorInlineSuggestionSession *session)
{
    return session != NULL ? session->revision : 0U;
}
