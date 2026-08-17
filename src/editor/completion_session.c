/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/completion_session.c
 *
 * PURPOSE:
 *   Implement completion navigation, details visibility, commit-character
 *   acceptance and revision-safe text-buffer application.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/completion_session.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorCompletionSession {
    UmiEditorCompletionCandidate *candidates;
    size_t count;
    size_t capacity;
    size_t selected_position;
    UmiEditorCompletionRequest request;
    uint64_t source_result_revision;
    uint64_t revision;
    UmiEditorCompletionSessionState state;
    int details_visible;
};

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

static UmiStatus reserve_candidates(UmiEditorCompletionSession *session,
                                    size_t required)
{
    size_t capacity;
    UmiEditorCompletionCandidate *replacement;

    if (required <= session->capacity) return UMI_STATUS_OK;
    capacity = session->capacity > 0U ? session->capacity : 32U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorCompletionCandidate *)realloc(
        session->candidates, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->candidates = replacement;
    session->capacity = capacity;
    return UMI_STATUS_OK;
}

static UmiStatus build_acceptance(
    const UmiEditorCompletionSession *session,
    int commit_character,
    int insert_commit_character,
    UmiEditorCompletionAcceptance *out_acceptance)
{
    const UmiEditorCompletionCandidate *candidate;
    size_t length;

    if (session == NULL || out_acceptance == NULL ||
        session->state != UMI_EDITOR_COMPLETION_SESSION_SHOWING ||
        session->selected_position >= session->count) {
        return UMI_STATUS_INVALID_STATE;
    }
    candidate = &session->candidates[session->selected_position];
    if (commit_character != 0 &&
        !umi_editor_completion_candidate_accepts_commit_character(
            candidate, commit_character)) {
        return UMI_STATUS_NOT_FOUND;
    }
    (void)memset(out_acceptance, 0, sizeof(*out_acceptance));
    out_acceptance->struct_size = (uint32_t)sizeof(*out_acceptance);
    out_acceptance->api_version = UMI_EDITOR_COMPLETION_SESSION_API_VERSION;
    out_acceptance->candidate = *candidate;
    copy_text(out_acceptance->inserted_text,
              sizeof(out_acceptance->inserted_text),
              candidate->item.insert_text);
    length = strlen(out_acceptance->inserted_text);
    if (commit_character != 0 && insert_commit_character &&
        (length == 0U ||
         (unsigned char)out_acceptance->inserted_text[length - 1U] !=
             (unsigned char)commit_character)) {
        if (length + 2U > sizeof(out_acceptance->inserted_text)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        out_acceptance->inserted_text[length] = (char)commit_character;
        out_acceptance->inserted_text[length + 1U] = '\0';
        out_acceptance->inserted_commit_character = 1;
    }
    copy_text(out_acceptance->command_id,
              sizeof(out_acceptance->command_id),
              candidate->command_id);
    out_acceptance->replace_start_offset = candidate->replace_start_offset;
    out_acceptance->replace_end_offset = candidate->replace_end_offset;
    out_acceptance->request_id = session->request.request_id;
    out_acceptance->source_document_revision =
        session->request.document_revision;
    out_acceptance->commit_character = commit_character;
    out_acceptance->snippet =
        candidate->insert_format == UMI_EDITOR_COMPLETION_INSERT_SNIPPET;
    out_acceptance->requires_confirmation =
        (candidate->flags &
         UMI_EDITOR_COMPLETION_CANDIDATE_REQUIRES_CONFIRMATION) != 0U;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_completion_session_create(
    UmiEditorCompletionSession **out_session)
{
    UmiEditorCompletionSession *session;

    if (out_session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_session = NULL;
    session = (UmiEditorCompletionSession *)calloc(1U, sizeof(*session));
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->revision = 1U;
    session->state = UMI_EDITOR_COMPLETION_SESSION_IDLE;
    *out_session = session;
    return UMI_STATUS_OK;
}

void umi_editor_completion_session_destroy(UmiEditorCompletionSession *session)
{
    if (session == NULL) return;
    free(session->candidates);
    session->candidates = NULL;
    free(session);
}

UmiStatus umi_editor_completion_session_begin(
    UmiEditorCompletionSession *session,
    const UmiEditorCompletionQueryResult *result,
    const UmiEditorCompletionRequest *request)
{
    const size_t count = umi_editor_completion_query_result_count(result);
    size_t position;
    int found_preselected = 0;
    UmiStatus status;

    if (session == NULL || result == NULL ||
        umi_editor_completion_request_validate(request) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = reserve_candidates(session, count);
    if (status != UMI_STATUS_OK) return status;
    session->count = 0U;
    session->selected_position = 0U;
    for (position = 0U; position < count; ++position) {
        UmiEditorCompletionMatch match;
        status = umi_editor_completion_query_result_at(result,
                                                        position,
                                                        &match);
        if (status != UMI_STATUS_OK) return status;
        session->candidates[session->count++] = match.candidate;
        if (!found_preselected &&
            (match.candidate.flags &
             UMI_EDITOR_COMPLETION_CANDIDATE_PRESELECTED) != 0U) {
            session->selected_position = position;
            found_preselected = 1;
        }
    }
    session->request = *request;
    session->source_result_revision =
        umi_editor_completion_query_result_revision(result);
    session->details_visible = 0;
    session->state = session->count > 0U
        ? UMI_EDITOR_COMPLETION_SESSION_SHOWING
        : UMI_EDITOR_COMPLETION_SESSION_EMPTY;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_completion_session_cancel(
    UmiEditorCompletionSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->state == UMI_EDITOR_COMPLETION_SESSION_ACCEPTED) {
        return UMI_STATUS_INVALID_STATE;
    }
    session->state = UMI_EDITOR_COMPLETION_SESSION_CANCELLED;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_completion_session_select(
    UmiEditorCompletionSession *session,
    size_t position)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->state != UMI_EDITOR_COMPLETION_SESSION_SHOWING) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (position >= session->count) return UMI_STATUS_NOT_FOUND;
    session->selected_position = position;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_completion_session_select_next(
    UmiEditorCompletionSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->state != UMI_EDITOR_COMPLETION_SESSION_SHOWING ||
        session->count == 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    session->selected_position =
        (session->selected_position + 1U) % session->count;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_completion_session_select_previous(
    UmiEditorCompletionSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->state != UMI_EDITOR_COMPLETION_SESSION_SHOWING ||
        session->count == 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    session->selected_position = session->selected_position == 0U
        ? session->count - 1U
        : session->selected_position - 1U;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_completion_session_select_page(
    UmiEditorCompletionSession *session,
    int direction,
    size_t page_size)
{
    size_t target;

    if (session == NULL || direction == 0 || page_size == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (session->state != UMI_EDITOR_COMPLETION_SESSION_SHOWING ||
        session->count == 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (direction > 0) {
        target = session->selected_position + page_size;
        if (target >= session->count) target = session->count - 1U;
    } else {
        target = page_size > session->selected_position
            ? 0U
            : session->selected_position - page_size;
    }
    session->selected_position = target;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_completion_session_toggle_details(
    UmiEditorCompletionSession *session)
{
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->state != UMI_EDITOR_COMPLETION_SESSION_SHOWING) {
        return UMI_STATUS_INVALID_STATE;
    }
    session->details_visible = !session->details_visible;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_completion_session_selected(
    const UmiEditorCompletionSession *session,
    UmiEditorCompletionCandidate *out_candidate)
{
    if (session == NULL || out_candidate == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (session->state != UMI_EDITOR_COMPLETION_SESSION_SHOWING ||
        session->selected_position >= session->count) {
        return UMI_STATUS_INVALID_STATE;
    }
    *out_candidate = session->candidates[session->selected_position];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_completion_session_replace_selected(
    UmiEditorCompletionSession *session,
    const UmiEditorCompletionCandidate *candidate)
{
    UmiEditorCompletionCandidate *selected;

    if (session == NULL ||
        umi_editor_completion_candidate_validate(candidate) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (session->state != UMI_EDITOR_COMPLETION_SESSION_SHOWING ||
        session->selected_position >= session->count) {
        return UMI_STATUS_INVALID_STATE;
    }
    selected = &session->candidates[session->selected_position];
    if (strcmp(selected->provider_id, candidate->provider_id) != 0 ||
        strcmp(selected->item.id, candidate->item.id) != 0 ||
        candidate->request_id != session->request.request_id) {
        return UMI_STATUS_INVALID_STATE;
    }
    *selected = *candidate;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_completion_session_accept(
    UmiEditorCompletionSession *session,
    int commit_character,
    int insert_commit_character,
    UmiEditorCompletionAcceptance *out_acceptance)
{
    UmiStatus status;

    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = build_acceptance(session,
                              commit_character,
                              insert_commit_character,
                              out_acceptance);
    if (status != UMI_STATUS_OK) return status;
    session->state = UMI_EDITOR_COMPLETION_SESSION_ACCEPTED;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_completion_session_apply(
    UmiEditorCompletionSession *session,
    UmiEditorTextBuffer *buffer,
    uint64_t current_document_revision,
    int commit_character,
    int insert_commit_character,
    UmiEditorCompletionAcceptance *out_acceptance)
{
    UmiEditorCompletionAcceptance acceptance;
    UmiEditorTextBufferView view;
    size_t start;
    size_t end;
    size_t inserted_count;
    UmiStatus status;

    if (session == NULL || buffer == NULL || out_acceptance == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = build_acceptance(session,
                              commit_character,
                              insert_commit_character,
                              &acceptance);
    if (status != UMI_STATUS_OK) return status;
    if ((session->request.document_revision != 0U &&
         current_document_revision != session->request.document_revision) ||
        (acceptance.candidate.document_revision != 0U &&
         current_document_revision != acceptance.candidate.document_revision)) {
        session->state = UMI_EDITOR_COMPLETION_SESSION_FAILED;
        session->revision = next_revision(session->revision);
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_editor_text_buffer_view(buffer, &view);
    if (status != UMI_STATUS_OK) return status;
    if (current_document_revision != view.revision ||
        acceptance.replace_start_offset > SIZE_MAX ||
        acceptance.replace_end_offset > SIZE_MAX) {
        session->state = UMI_EDITOR_COMPLETION_SESSION_FAILED;
        session->revision = next_revision(session->revision);
        return UMI_STATUS_INVALID_STATE;
    }
    start = (size_t)acceptance.replace_start_offset;
    end = (size_t)acceptance.replace_end_offset;
    if (end < start || start > view.byte_count || end > view.byte_count) {
        session->state = UMI_EDITOR_COMPLETION_SESSION_FAILED;
        session->revision = next_revision(session->revision);
        return UMI_STATUS_INVALID_STATE;
    }
    inserted_count = strlen(acceptance.inserted_text);
    status = umi_editor_text_buffer_replace(buffer,
                                            start,
                                            end - start,
                                            acceptance.inserted_text,
                                            inserted_count);
    if (status != UMI_STATUS_OK) {
        session->state = UMI_EDITOR_COMPLETION_SESSION_FAILED;
        session->revision = next_revision(session->revision);
        return status;
    }
    acceptance.resulting_document_revision =
        umi_editor_text_buffer_revision(buffer);
    *out_acceptance = acceptance;
    session->state = UMI_EDITOR_COMPLETION_SESSION_ACCEPTED;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_completion_session_snapshot(
    const UmiEditorCompletionSession *session,
    UmiEditorCompletionSessionSnapshot *out_snapshot)
{
    if (session == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_COMPLETION_SESSION_API_VERSION;
    out_snapshot->state = session->state;
    out_snapshot->candidate_count = session->count;
    out_snapshot->selected_position = session->selected_position;
    out_snapshot->request_id = session->request.request_id;
    out_snapshot->document_revision = session->request.document_revision;
    out_snapshot->source_result_revision = session->source_result_revision;
    out_snapshot->revision = session->revision;
    out_snapshot->details_visible = session->details_visible;
    out_snapshot->can_accept =
        session->state == UMI_EDITOR_COMPLETION_SESSION_SHOWING &&
        session->selected_position < session->count;
    return UMI_STATUS_OK;
}

size_t umi_editor_completion_session_count(
    const UmiEditorCompletionSession *session)
{
    return session != NULL ? session->count : 0U;
}

uint64_t umi_editor_completion_session_revision(
    const UmiEditorCompletionSession *session)
{
    return session != NULL ? session->revision : 0U;
}
