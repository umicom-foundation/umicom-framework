/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/inline_suggestion_session.h
 *
 * PURPOSE:
 *   Coordinate inline-suggestion providers, ghost-text navigation, streaming,
 *   typed-through updates and revision-safe full/word/line acceptance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * List the named editor inline suggestion session state values accepted by this public
 * contract.
 */
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

/**
 * List the named editor inline suggestion acceptance kind values accepted by this public
 * contract.
 */
typedef enum UmiEditorInlineSuggestionAcceptanceKind {
    UMI_EDITOR_INLINE_SUGGESTION_ACCEPT_ALL = 1,
    UMI_EDITOR_INLINE_SUGGESTION_ACCEPT_WORD = 2,
    UMI_EDITOR_INLINE_SUGGESTION_ACCEPT_LINE = 3
} UmiEditorInlineSuggestionAcceptanceKind;

/**
 * Represent the editor inline suggestion policy data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the editor inline suggestion provider report data shared with callers of this
 * public contract.
 */
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

/**
 * Represent the editor inline suggestion acceptance data shared with callers of this
 * public contract.
 */
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

/**
 * Represent the editor inline suggestion session snapshot data shared with callers of this
 * public contract.
 */
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

/**
 * Represent the editor inline suggestion session data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorInlineSuggestionSession
    UmiEditorInlineSuggestionSession;

/**
 * Provide the editor inline suggestion policy default operation used by this module and
 * its client applications.
 */
UmiEditorInlineSuggestionPolicy umi_editor_inline_suggestion_policy_default(
    void);
/**
 * Initialise editor inline suggestion session from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_inline_suggestion_session_create(
    UmiEditorInlineSuggestionProviderRegistry *provider_registry,
    UmiEditorInlineSuggestionSession **out_session);
/**
 * Release or reset state held by editor inline suggestion session so the same storage can
 * be reused safely.
 */
void umi_editor_inline_suggestion_session_destroy(
    UmiEditorInlineSuggestionSession *session);
/**
 * Provide the editor inline suggestion session set policy operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_set_policy(
    UmiEditorInlineSuggestionSession *session,
    const UmiEditorInlineSuggestionPolicy *policy);
/**
 * Provide the editor inline suggestion session begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_begin(
    UmiEditorInlineSuggestionSession *session,
    const UmiEditorCompletionRequest *request);
/**
 * Provide the editor inline suggestion session cancel operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_cancel(
    UmiEditorInlineSuggestionSession *session);
/**
 * Provide the editor inline suggestion session select next operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_select_next(
    UmiEditorInlineSuggestionSession *session);
/**
 * Provide the editor inline suggestion session select previous operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_select_previous(
    UmiEditorInlineSuggestionSession *session);
/**
 * Provide the editor inline suggestion session current operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_current(
    const UmiEditorInlineSuggestionSession *session,
    UmiEditorInlineSuggestion *out_suggestion);
/**
 * Provide the editor inline suggestion session append chunk operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_append_chunk(
    UmiEditorInlineSuggestionSession *session,
    const char *provider_id,
    const char *suggestion_id,
    const char *chunk,
    int complete);
/**
 * Provide the editor inline suggestion session typed through operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_typed_through(
    UmiEditorInlineSuggestionSession *session,
    const char *typed_text,
    uint64_t current_document_revision);
/**
 * Provide the editor inline suggestion session accept operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_accept(
    UmiEditorInlineSuggestionSession *session,
    UmiEditorTextBuffer *buffer,
    uint64_t current_document_revision,
    UmiEditorInlineSuggestionAcceptanceKind kind,
    UmiEditorInlineSuggestionAcceptance *out_acceptance);
/**
 * Provide the editor inline suggestion session reject operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_reject(
    UmiEditorInlineSuggestionSession *session);
/**
 * Find editor inline suggestion session report while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_inline_suggestion_session_report_at(
    const UmiEditorInlineSuggestionSession *session,
    size_t position,
    UmiEditorInlineSuggestionProviderReport *out_report);
/**
 * Provide the editor inline suggestion session snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_inline_suggestion_session_snapshot(
    const UmiEditorInlineSuggestionSession *session,
    UmiEditorInlineSuggestionSessionSnapshot *out_snapshot);
/**
 * Provide the editor inline suggestion session revision operation used by this module and
 * its client applications.
 */
uint64_t umi_editor_inline_suggestion_session_revision(
    const UmiEditorInlineSuggestionSession *session);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_INLINE_SUGGESTION_SESSION_H */
