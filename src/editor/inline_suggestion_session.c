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

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

static UmiStatus validate_policy(
    const UmiEditorInlineSuggestionPolicy *policy)
{
    if (policy == NULL ||
        policy->struct_size != (uint32_t)sizeof(*policy) ||
        policy->api_version != UMI_EDITOR_INLINE_SUGGESTION_SESSION_API_VERSION ||
        policy->maximum_providers == 0U ||
        policy->maximum_suggestions == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus reserve_suggestions(UmiEditorInlineSuggestionSession *session,
                                     size_t required)
{
    size_t capacity;
    UmiEditorInlineSuggestion *replacement;

    if (required <= session->capacity) return UMI_STATUS_OK;
    capacity = session->capacity > 0U ? session->capacity : 8U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorInlineSuggestion *)realloc(
        session->suggestions, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->suggestions = replacement;
    session->capacity = capacity;
    return UMI_STATUS_OK;
}

static UmiStatus reserve_reports(UmiEditorInlineSuggestionSession *session,
                                 size_t required)
{
    size_t capacity;
    UmiEditorInlineSuggestionProviderReport *replacement;

    if (required <= session->report_capacity) return UMI_STATUS_OK;
    capacity = session->report_capacity > 0U ? session->report_capacity : 8U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorInlineSuggestionProviderReport *)realloc(
        session->reports, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->reports = replacement;
    session->report_capacity = capacity;
    return UMI_STATUS_OK;
}

static UmiStatus add_report(
    UmiEditorInlineSuggestionSession *session,
    const UmiEditorInlineSuggestionProviderReport *report)
{
    UmiStatus status = reserve_reports(session, session->report_count + 1U);
    if (status != UMI_STATUS_OK) return status;
    session->reports[session->report_count++] = *report;
    return UMI_STATUS_OK;
}

static int provider_blocked_by_policy(
    const UmiEditorInlineSuggestionProviderDescriptor *descriptor,
    const UmiEditorInlineSuggestionPolicy *policy,
    const UmiEditorCompletionRequest *request)
{
    if (descriptor->source == UMI_EDITOR_COMPLETION_SOURCE_AI &&
        (!policy->allow_ai || !request->allow_ai)) {
        return 1;
    }
    if (descriptor->requires_network &&
        (!policy->allow_remote || !request->allow_remote)) {
        return 1;
    }
    if (descriptor->requires_workspace_trust &&
        (!policy->trusted_workspace || !request->trusted_workspace)) {
        return 1;
    }
    return 0;
}

static UmiStatus collect_suggestion(
    const UmiEditorInlineSuggestion *suggestion,
    void *user_data)
{
    InlineSinkContext *context = (InlineSinkContext *)user_data;
    UmiEditorInlineSuggestion stored;
    UmiStatus status;

    if (context == NULL || suggestion == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_editor_completion_request_is_cancelled(&context->session->request)) {
        return UMI_STATUS_CANCELLED;
    }
    {
        size_t maximum_suggestions =
            context->session->policy.maximum_suggestions;
        if (context->session->request.maximum_candidates <
            maximum_suggestions) {
            maximum_suggestions =
                context->session->request.maximum_candidates;
        }
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
    if (stored.provider_id[0] == '\0') {
        copy_text(stored.provider_id,
                  sizeof(stored.provider_id),
                  context->descriptor.id);
    }
    if (stored.document_id[0] == '\0') {
        copy_text(stored.document_id,
                  sizeof(stored.document_id),
                  context->session->request.document_id);
    }
    if (stored.request_id == 0U) {
        stored.request_id = context->session->request.request_id;
    }
    if (stored.document_revision == 0U) {
        stored.document_revision = context->session->request.document_revision;
    }
    stored.provider_priority = context->descriptor.priority;
    if (stored.source == UMI_EDITOR_COMPLETION_SOURCE_OTHER) {
        stored.source = context->descriptor.source;
    }
    if (context->descriptor.source == UMI_EDITOR_COMPLETION_SOURCE_AI) {
        stored.flags |= UMI_EDITOR_INLINE_SUGGESTION_AI;
    }
    if (context->descriptor.requires_network) {
        stored.flags |= UMI_EDITOR_INLINE_SUGGESTION_REMOTE;
    }
    if (context->descriptor.requires_workspace_trust) {
        stored.flags |= UMI_EDITOR_INLINE_SUGGESTION_REQUIRES_TRUST;
    }
    status = umi_editor_inline_suggestion_collection_upsert(
        context->session->collection, &stored);
    if (status == UMI_STATUS_OK) ++context->emitted_count;
    return status;
}

static int suggestion_compare(const void *left_pointer,
                              const void *right_pointer)
{
    const UmiEditorInlineSuggestion *left =
        (const UmiEditorInlineSuggestion *)left_pointer;
    const UmiEditorInlineSuggestion *right =
        (const UmiEditorInlineSuggestion *)right_pointer;
    int order;

    if (left->provider_priority > right->provider_priority) return -1;
    if (left->provider_priority < right->provider_priority) return 1;
    if (left->confidence_milli > right->confidence_milli) return -1;
    if (left->confidence_milli < right->confidence_milli) return 1;
    order = strcmp(left->provider_id, right->provider_id);
    if (order != 0) return order;
    return strcmp(left->id, right->id);
}

static size_t find_session_suggestion(
    const UmiEditorInlineSuggestionSession *session,
    const char *provider_id,
    const char *suggestion_id)
{
    size_t position;

    if (session == NULL || provider_id == NULL || suggestion_id == NULL) {
        return SIZE_MAX;
    }
    for (position = 0U; position < session->count; ++position) {
        if (strcmp(session->suggestions[position].provider_id, provider_id) == 0 &&
            strcmp(session->suggestions[position].id, suggestion_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

static size_t acceptance_length(const char *text,
                                UmiEditorInlineSuggestionAcceptanceKind kind)
{
    const size_t length = strlen(text);
    size_t position = 0U;

    if (kind == UMI_EDITOR_INLINE_SUGGESTION_ACCEPT_ALL) return length;
    if (kind == UMI_EDITOR_INLINE_SUGGESTION_ACCEPT_LINE) {
        const char *newline = strchr(text, '\n');
        return newline != NULL ? (size_t)(newline - text) + 1U : length;
    }
    while (position < length &&
           isspace((int)(unsigned char)text[position]) != 0) {
        ++position;
    }
    if (position >= length) return length;
    if (isalnum((int)(unsigned char)text[position]) != 0 ||
        text[position] == '_') {
        while (position < length &&
               (isalnum((int)(unsigned char)text[position]) != 0 ||
                text[position] == '_')) {
            ++position;
        }
    } else {
        ++position;
    }
    return position;
}

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

UmiStatus umi_editor_inline_suggestion_session_create(
    UmiEditorInlineSuggestionProviderRegistry *provider_registry,
    UmiEditorInlineSuggestionSession **out_session)
{
    UmiEditorInlineSuggestionSession *session;
    UmiStatus status;

    if (provider_registry == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_session = NULL;
    session = (UmiEditorInlineSuggestionSession *)calloc(1U, sizeof(*session));
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->provider_registry = provider_registry;
    session->policy = umi_editor_inline_suggestion_policy_default();
    session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_IDLE;
    session->revision = 1U;
    status = umi_editor_inline_suggestion_collection_create(&session->collection);
    if (status != UMI_STATUS_OK) {
        free(session);
        return status;
    }
    *out_session = session;
    return UMI_STATUS_OK;
}

void umi_editor_inline_suggestion_session_destroy(
    UmiEditorInlineSuggestionSession *session)
{
    if (session == NULL) return;
    umi_editor_inline_suggestion_collection_destroy(session->collection);
    free(session->suggestions);
    session->suggestions = NULL;
    free(session->reports);
    session->reports = NULL;
    free(session);
}

UmiStatus umi_editor_inline_suggestion_session_set_policy(
    UmiEditorInlineSuggestionSession *session,
    const UmiEditorInlineSuggestionPolicy *policy)
{
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

    for (position = 0U; position < provider_count; ++position) {
        UmiEditorInlineSuggestionProviderDescriptor descriptor;
        UmiEditorInlineSuggestionProviderReport report;
        UmiEditorInlineSuggestionProviderResponse response;
        InlineSinkContext context;

        if (umi_editor_completion_request_is_cancelled(request)) {
            session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_CANCELLED;
            session->revision = next_revision(session->revision);
            return UMI_STATUS_CANCELLED;
        }
        status = umi_editor_inline_suggestion_provider_registry_at(
            session->provider_registry, position, &descriptor);
        if (status != UMI_STATUS_OK) continue;
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
        if (provider_blocked_by_policy(&descriptor,
                                       &session->policy,
                                       request) ||
            session->invoked_provider_count >= session->policy.maximum_providers) {
            report.status = UMI_STATUS_PERMISSION_DENIED;
            report.skipped_by_policy = 1;
            if (session->invoked_provider_count >=
                session->policy.maximum_providers) {
                report.truncated = 1;
                session->truncated = 1;
            }
            status = add_report(session, &report);
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
        if (status == UMI_STATUS_CAPACITY_EXCEEDED && context.truncated) {
            status = UMI_STATUS_OK;
        }
        report.status = status;
        report.emitted_suggestion_count = context.emitted_count;
        report.provider_revision = response.provider_revision;
        report.incomplete = response.incomplete;
        report.streaming = response.streaming;
        report.truncated = context.truncated;
        if (response.incomplete) session->incomplete = 1;
        if (status != UMI_STATUS_OK) ++session->failed_provider_count;
        status = add_report(session, &report);
        if (status != UMI_STATUS_OK) return status;
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
    if (status != UMI_STATUS_OK) return status;
    for (position = 0U; position < session->count; ++position) {
        status = umi_editor_inline_suggestion_collection_at(
            session->collection, position, &session->suggestions[position]);
        if (status != UMI_STATUS_OK) return status;
    }
    if (session->count > 1U) {
        qsort(session->suggestions,
              session->count,
              sizeof(*session->suggestions),
              suggestion_compare);
    }
    if (session->count == 0U) {
        session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_EMPTY;
    } else if ((session->suggestions[0].flags &
                UMI_EDITOR_INLINE_SUGGESTION_STREAMING) != 0U) {
        session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_STREAMING;
    } else {
        session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_SHOWING;
    }
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_inline_suggestion_session_cancel(
    UmiEditorInlineSuggestionSession *session)
{
    size_t position;

    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (position = 0U; position < session->report_count; ++position) {
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

UmiStatus umi_editor_inline_suggestion_session_select_next(
    UmiEditorInlineSuggestionSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
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

UmiStatus umi_editor_inline_suggestion_session_select_previous(
    UmiEditorInlineSuggestionSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
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

UmiStatus umi_editor_inline_suggestion_session_current(
    const UmiEditorInlineSuggestionSession *session,
    UmiEditorInlineSuggestion *out_suggestion)
{
    if (session == NULL || out_suggestion == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((session->state != UMI_EDITOR_INLINE_SUGGESTION_SESSION_SHOWING &&
         session->state != UMI_EDITOR_INLINE_SUGGESTION_SESSION_STREAMING) ||
        session->selected_position >= session->count) {
        return UMI_STATUS_INVALID_STATE;
    }
    *out_suggestion = session->suggestions[session->selected_position];
    return UMI_STATUS_OK;
}

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

    if (session == NULL || provider_id == NULL || suggestion_id == NULL ||
        chunk == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_session_suggestion(session, provider_id, suggestion_id);
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    suggestion = &session->suggestions[position];
    existing_length = strlen(suggestion->text);
    chunk_length = strlen(chunk);
    if (chunk_length >= sizeof(suggestion->text) - existing_length) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (chunk_length > 0U) {
        (void)memcpy(suggestion->text + existing_length, chunk, chunk_length);
    }
    suggestion->text[existing_length + chunk_length] = '\0';
    if (complete) {
        suggestion->flags &=
            ~(UmiEditorInlineSuggestionFlags)
                UMI_EDITOR_INLINE_SUGGESTION_STREAMING;
        suggestion->flags |= UMI_EDITOR_INLINE_SUGGESTION_COMPLETE;
    } else {
        suggestion->flags |= UMI_EDITOR_INLINE_SUGGESTION_STREAMING;
    }
    if (position == session->selected_position) {
        session->state = complete
            ? UMI_EDITOR_INLINE_SUGGESTION_SESSION_SHOWING
            : UMI_EDITOR_INLINE_SUGGESTION_SESSION_STREAMING;
    }
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_inline_suggestion_session_typed_through(
    UmiEditorInlineSuggestionSession *session,
    const char *typed_text,
    uint64_t current_document_revision)
{
    UmiEditorInlineSuggestion *suggestion;
    size_t typed_length;
    size_t remaining_length;

    if (session == NULL || typed_text == NULL || typed_text[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((session->state != UMI_EDITOR_INLINE_SUGGESTION_SESSION_SHOWING &&
         session->state != UMI_EDITOR_INLINE_SUGGESTION_SESSION_STREAMING) ||
        session->selected_position >= session->count) {
        return UMI_STATUS_INVALID_STATE;
    }
    suggestion = &session->suggestions[session->selected_position];
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
    if (remaining_length == 0U) {
        session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_ACCEPTED;
    }
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

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

    if (session == NULL || buffer == NULL || out_acceptance == NULL ||
        kind < UMI_EDITOR_INLINE_SUGGESTION_ACCEPT_ALL ||
        kind > UMI_EDITOR_INLINE_SUGGESTION_ACCEPT_LINE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((session->state != UMI_EDITOR_INLINE_SUGGESTION_SESSION_SHOWING &&
         session->state != UMI_EDITOR_INLINE_SUGGESTION_SESSION_STREAMING) ||
        session->selected_position >= session->count) {
        return UMI_STATUS_INVALID_STATE;
    }
    suggestion = &session->suggestions[session->selected_position];
    if (kind != UMI_EDITOR_INLINE_SUGGESTION_ACCEPT_ALL) {
        status = umi_editor_inline_suggestion_provider_registry_find(
            session->provider_registry,
            suggestion->provider_id,
            &descriptor);
        if (status != UMI_STATUS_OK) return status;
        if (!descriptor.supports_partial_acceptance) {
            return UMI_STATUS_NOT_IMPLEMENTED;
        }
        if (suggestion->replace_start_offset !=
            suggestion->replace_end_offset) {
            return UMI_STATUS_INVALID_STATE;
        }
    }
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
    if (status != UMI_STATUS_OK) return status;
    if (view.revision != current_document_revision) {
        session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_FAILED;
        session->revision = next_revision(session->revision);
        return UMI_STATUS_INVALID_STATE;
    }
    start = (size_t)suggestion->replace_start_offset;
    end = (size_t)suggestion->replace_end_offset;
    if (end < start || start > view.byte_count || end > view.byte_count) {
        session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_FAILED;
        session->revision = next_revision(session->revision);
        return UMI_STATUS_INVALID_STATE;
    }
    full_length = strlen(suggestion->text);
    accepted_length = acceptance_length(suggestion->text, kind);
    if (accepted_length == 0U || accepted_length > full_length) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_editor_text_buffer_replace(buffer,
                                            start,
                                            end - start,
                                            suggestion->text,
                                            accepted_length);
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

    if (remaining_length == 0U) {
        suggestion->text[0] = '\0';
        session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_ACCEPTED;
    } else {
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

UmiStatus umi_editor_inline_suggestion_session_reject(
    UmiEditorInlineSuggestionSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->state != UMI_EDITOR_INLINE_SUGGESTION_SESSION_SHOWING &&
        session->state != UMI_EDITOR_INLINE_SUGGESTION_SESSION_STREAMING) {
        return UMI_STATUS_INVALID_STATE;
    }
    session->state = UMI_EDITOR_INLINE_SUGGESTION_SESSION_REJECTED;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_inline_suggestion_session_report_at(
    const UmiEditorInlineSuggestionSession *session,
    size_t position,
    UmiEditorInlineSuggestionProviderReport *out_report)
{
    if (session == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (position >= session->report_count) return UMI_STATUS_NOT_FOUND;
    *out_report = session->reports[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_inline_suggestion_session_snapshot(
    const UmiEditorInlineSuggestionSession *session,
    UmiEditorInlineSuggestionSessionSnapshot *out_snapshot)
{
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

uint64_t umi_editor_inline_suggestion_session_revision(
    const UmiEditorInlineSuggestionSession *session)
{
    return session != NULL ? session->revision : 0U;
}
