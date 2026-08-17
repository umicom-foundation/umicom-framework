/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/inline_suggestion_session.h
 *
 * PURPOSE:
 *   Coordinate inline-suggestion providers, ghost-text navigation, streaming,
 *   typed-through updates and revision-safe full/word/line acceptance.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INLINE_SUGGESTION_SESSION_H
#define UMICOM_EDITOR_INLINE_SUGGESTION_SESSION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/inline_suggestion.h"
#include "umicom/editor/text_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_INLINE_SUGGESTION_SESSION_API_VERSION 1U

typedef enum UmiEditorInlineSuggestionSessionState {
    UMI_EDITOR_INLINE_SUGGESTION_SESSION_IDLE = 0,
    UMI_EDITOR_INLINE_SUGGESTION_SESSION_COLLECTING = 1,
    UMI_EDITOR_INLINE_SUGGESTION_SESSION_EMPTY = 2,
    UMI_EDITOR_INLINE_SUGGESTION_SESSION_SHOWING = 3,
    UMI_EDITOR_INLINE_SUGGESTION_SESSION_STREAMING = 4,
    UMI_EDITOR_INLINE_SUGGESTION_SESSION_ACCEPTED = 5,
    UMI_EDITOR_INLINE_SUGGESTION_SESSION_REJECTED = 6,
    UMI_EDITOR_INLINE_SUGGESTION_SESSION_CANCELLED = 7,
    UMI_EDITOR_INLINE_SUGGESTION_SESSION_FAILED = 8
} UmiEditorInlineSuggestionSessionState;

typedef enum UmiEditorInlineSuggestionAcceptanceKind {
    UMI_EDITOR_INLINE_SUGGESTION_ACCEPT_ALL = 1,
    UMI_EDITOR_INLINE_SUGGESTION_ACCEPT_WORD = 2,
    UMI_EDITOR_INLINE_SUGGESTION_ACCEPT_LINE = 3
} UmiEditorInlineSuggestionAcceptanceKind;

typedef struct UmiEditorInlineSuggestionPolicy {
    uint32_t struct_size;
    uint32_t api_version;
    size_t maximum_providers;
    size_t maximum_suggestions;
    int allow_ai;
    int allow_remote;
    int trusted_workspace;
    int continue_on_provider_error;
} UmiEditorInlineSuggestionPolicy;

typedef struct UmiEditorInlineSuggestionProviderReport {
    uint32_t struct_size;
    uint32_t api_version;
    char provider_id[UMI_EDITOR_COMPLETION_PROVIDER_ID_CAPACITY];
    UmiStatus status;
    size_t emitted_suggestion_count;
    uint64_t provider_revision;
    int incomplete;
    int streaming;
    int skipped_by_policy;
    int truncated;
} UmiEditorInlineSuggestionProviderReport;

typedef struct UmiEditorInlineSuggestionAcceptance {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorInlineSuggestion suggestion;
    char inserted_text[UMI_EDITOR_INLINE_SUGGESTION_TEXT_CAPACITY];
    size_t inserted_byte_count;
    size_t remaining_byte_count;
    uint64_t resulting_document_revision;
    UmiEditorInlineSuggestionAcceptanceKind kind;
    int complete;
} UmiEditorInlineSuggestionAcceptance;

typedef struct UmiEditorInlineSuggestionSessionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorInlineSuggestionSessionState state;
    size_t suggestion_count;
    size_t selected_position;
    size_t invoked_provider_count;
    size_t failed_provider_count;
    uint64_t request_id;
    uint64_t document_revision;
    uint64_t provider_registry_revision;
    uint64_t revision;
    int truncated;
    int incomplete;
    int can_accept;
} UmiEditorInlineSuggestionSessionSnapshot;

typedef struct UmiEditorInlineSuggestionSession
    UmiEditorInlineSuggestionSession;

UmiEditorInlineSuggestionPolicy umi_editor_inline_suggestion_policy_default(
    void);
UmiStatus umi_editor_inline_suggestion_session_create(
    UmiEditorInlineSuggestionProviderRegistry *provider_registry,
    UmiEditorInlineSuggestionSession **out_session);
void umi_editor_inline_suggestion_session_destroy(
    UmiEditorInlineSuggestionSession *session);
UmiStatus umi_editor_inline_suggestion_session_set_policy(
    UmiEditorInlineSuggestionSession *session,
    const UmiEditorInlineSuggestionPolicy *policy);
UmiStatus umi_editor_inline_suggestion_session_begin(
    UmiEditorInlineSuggestionSession *session,
    const UmiEditorCompletionRequest *request);
UmiStatus umi_editor_inline_suggestion_session_cancel(
    UmiEditorInlineSuggestionSession *session);
UmiStatus umi_editor_inline_suggestion_session_select_next(
    UmiEditorInlineSuggestionSession *session);
UmiStatus umi_editor_inline_suggestion_session_select_previous(
    UmiEditorInlineSuggestionSession *session);
UmiStatus umi_editor_inline_suggestion_session_current(
    const UmiEditorInlineSuggestionSession *session,
    UmiEditorInlineSuggestion *out_suggestion);
UmiStatus umi_editor_inline_suggestion_session_append_chunk(
    UmiEditorInlineSuggestionSession *session,
    const char *provider_id,
    const char *suggestion_id,
    const char *chunk,
    int complete);
UmiStatus umi_editor_inline_suggestion_session_typed_through(
    UmiEditorInlineSuggestionSession *session,
    const char *typed_text,
    uint64_t current_document_revision);
UmiStatus umi_editor_inline_suggestion_session_accept(
    UmiEditorInlineSuggestionSession *session,
    UmiEditorTextBuffer *buffer,
    uint64_t current_document_revision,
    UmiEditorInlineSuggestionAcceptanceKind kind,
    UmiEditorInlineSuggestionAcceptance *out_acceptance);
UmiStatus umi_editor_inline_suggestion_session_reject(
    UmiEditorInlineSuggestionSession *session);
UmiStatus umi_editor_inline_suggestion_session_report_at(
    const UmiEditorInlineSuggestionSession *session,
    size_t position,
    UmiEditorInlineSuggestionProviderReport *out_report);
UmiStatus umi_editor_inline_suggestion_session_snapshot(
    const UmiEditorInlineSuggestionSession *session,
    UmiEditorInlineSuggestionSessionSnapshot *out_snapshot);
uint64_t umi_editor_inline_suggestion_session_revision(
    const UmiEditorInlineSuggestionSession *session);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_INLINE_SUGGESTION_SESSION_H */
