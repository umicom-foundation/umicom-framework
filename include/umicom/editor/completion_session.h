/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/completion_session.h
 *
 * PURPOSE:
 *   Own completion-popup selection, navigation, details and revision-safe
 *   acceptance, including commit-character and snippet hand-off semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_COMPLETION_SESSION_H
#define UMICOM_EDITOR_COMPLETION_SESSION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/completion_query.h"
#include "umicom/editor/text_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_COMPLETION_SESSION_API_VERSION 1U

typedef enum UmiEditorCompletionSessionState {
    UMI_EDITOR_COMPLETION_SESSION_IDLE = 0,
    UMI_EDITOR_COMPLETION_SESSION_EMPTY = 1,
    UMI_EDITOR_COMPLETION_SESSION_SHOWING = 2,
    UMI_EDITOR_COMPLETION_SESSION_ACCEPTED = 3,
    UMI_EDITOR_COMPLETION_SESSION_CANCELLED = 4,
    UMI_EDITOR_COMPLETION_SESSION_FAILED = 5
} UmiEditorCompletionSessionState;

typedef struct UmiEditorCompletionAcceptance {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorCompletionCandidate candidate;
    char inserted_text[1026U];
    char command_id[UMI_EDITOR_COMPLETION_COMMAND_ID_CAPACITY];
    uint64_t replace_start_offset;
    uint64_t replace_end_offset;
    uint64_t request_id;
    uint64_t source_document_revision;
    uint64_t resulting_document_revision;
    int commit_character;
    int inserted_commit_character;
    int snippet;
    int requires_confirmation;
} UmiEditorCompletionAcceptance;

typedef struct UmiEditorCompletionSessionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorCompletionSessionState state;
    size_t candidate_count;
    size_t selected_position;
    uint64_t request_id;
    uint64_t document_revision;
    uint64_t source_result_revision;
    uint64_t revision;
    int details_visible;
    int can_accept;
} UmiEditorCompletionSessionSnapshot;

typedef struct UmiEditorCompletionSession UmiEditorCompletionSession;

UmiStatus umi_editor_completion_session_create(
    UmiEditorCompletionSession **out_session);
void umi_editor_completion_session_destroy(UmiEditorCompletionSession *session);
UmiStatus umi_editor_completion_session_begin(
    UmiEditorCompletionSession *session,
    const UmiEditorCompletionQueryResult *result,
    const UmiEditorCompletionRequest *request);
UmiStatus umi_editor_completion_session_cancel(
    UmiEditorCompletionSession *session);
UmiStatus umi_editor_completion_session_select(
    UmiEditorCompletionSession *session,
    size_t position);
UmiStatus umi_editor_completion_session_select_next(
    UmiEditorCompletionSession *session);
UmiStatus umi_editor_completion_session_select_previous(
    UmiEditorCompletionSession *session);
UmiStatus umi_editor_completion_session_select_page(
    UmiEditorCompletionSession *session,
    int direction,
    size_t page_size);
UmiStatus umi_editor_completion_session_toggle_details(
    UmiEditorCompletionSession *session);
UmiStatus umi_editor_completion_session_selected(
    const UmiEditorCompletionSession *session,
    UmiEditorCompletionCandidate *out_candidate);
UmiStatus umi_editor_completion_session_replace_selected(
    UmiEditorCompletionSession *session,
    const UmiEditorCompletionCandidate *candidate);
UmiStatus umi_editor_completion_session_accept(
    UmiEditorCompletionSession *session,
    int commit_character,
    int insert_commit_character,
    UmiEditorCompletionAcceptance *out_acceptance);
UmiStatus umi_editor_completion_session_apply(
    UmiEditorCompletionSession *session,
    UmiEditorTextBuffer *buffer,
    uint64_t current_document_revision,
    int commit_character,
    int insert_commit_character,
    UmiEditorCompletionAcceptance *out_acceptance);
UmiStatus umi_editor_completion_session_snapshot(
    const UmiEditorCompletionSession *session,
    UmiEditorCompletionSessionSnapshot *out_snapshot);
size_t umi_editor_completion_session_count(
    const UmiEditorCompletionSession *session);
uint64_t umi_editor_completion_session_revision(
    const UmiEditorCompletionSession *session);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_COMPLETION_SESSION_H */
