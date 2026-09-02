/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/formatting_session.h
 *
 * PURPOSE:
 *   Coordinate revision-safe document, range, paste, save and on-type
 *   formatting operations through canonical Framework workspace edits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_FORMATTING_SESSION_H
#define UMICOM_EDITOR_FORMATTING_SESSION_H

#include "umicom/editor/formatting_profile.h"
#include "umicom/editor/workspace_edit.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_FORMATTING_SESSION_API_VERSION 1U
#define UMI_EDITOR_FORMATTING_REQUEST_ID_CAPACITY 128U
#define UMI_EDITOR_FORMATTING_REQUEST_PROVIDER_CAPACITY 128U
#define UMI_EDITOR_FORMATTING_REQUEST_URI_CAPACITY 1024U
#define UMI_EDITOR_FORMATTING_REQUEST_PROFILE_CAPACITY 128U

/**
 * List the named editor formatting request mode values accepted by this public contract.
 */
typedef enum UmiEditorFormattingRequestMode {
    UMI_EDITOR_FORMATTING_DOCUMENT = 1,
    UMI_EDITOR_FORMATTING_RANGE = 2,
    UMI_EDITOR_FORMATTING_ON_TYPE = 3,
    UMI_EDITOR_FORMATTING_ON_SAVE = 4,
    UMI_EDITOR_FORMATTING_ON_PASTE = 5
} UmiEditorFormattingRequestMode;

/**
 * List the named editor formatting session state values accepted by this public contract.
 */
typedef enum UmiEditorFormattingSessionState {
    UMI_EDITOR_FORMATTING_IDLE = 0,
    UMI_EDITOR_FORMATTING_COLLECTING = 1,
    UMI_EDITOR_FORMATTING_READY = 2,
    UMI_EDITOR_FORMATTING_APPLIED = 3,
    UMI_EDITOR_FORMATTING_CONFLICT = 4,
    UMI_EDITOR_FORMATTING_CANCELLED = 5,
    UMI_EDITOR_FORMATTING_FAILED = 6
} UmiEditorFormattingSessionState;

/**
 * Represent the editor formatting request data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorFormattingRequest {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_FORMATTING_REQUEST_ID_CAPACITY];
    char provider_id[UMI_EDITOR_FORMATTING_REQUEST_PROVIDER_CAPACITY];
    char document_uri[UMI_EDITOR_FORMATTING_REQUEST_URI_CAPACITY];
    char profile_id[UMI_EDITOR_FORMATTING_REQUEST_PROFILE_CAPACITY];
    UmiEditorFormattingRequestMode mode;
    uint32_t trigger_character;
    uint64_t start_byte_offset;
    uint64_t end_byte_offset;
    uint64_t document_revision;
    int require_matching_revision;
} UmiEditorFormattingRequest;

/**
 * Represent the editor formatting session snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorFormattingSessionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorFormattingRequest request;
    UmiEditorFormattingSessionState state;
    size_t edit_count;
    size_t conflict_count;
    size_t applied_count;
    uint64_t revision;
} UmiEditorFormattingSessionSnapshot;

/**
 * Represent the editor formatting session data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorFormattingSession UmiEditorFormattingSession;

/**
 * Initialise editor formatting session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_formatting_session_create(
    UmiEditorFormattingSession **out_session);
/**
 * Release or reset state held by editor formatting session so the same storage can be
 * reused safely.
 */
void umi_editor_formatting_session_destroy(UmiEditorFormattingSession *session);
/**
 * Provide the editor formatting session begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_formatting_session_begin(
    UmiEditorFormattingSession *session,
    const UmiEditorFormattingRequest *request);
/**
 * Provide the editor formatting session add edit operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_formatting_session_add_edit(
    UmiEditorFormattingSession *session,
    const UmiEditorWorkspaceTextEdit *edit);
/**
 * Provide the editor formatting session finalize operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_formatting_session_finalize(
    UmiEditorFormattingSession *session);
/**
 * Perform editor formatting session through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_editor_formatting_session_apply(
    UmiEditorFormattingSession *session,
    UmiEditorTextBuffer *buffer,
    size_t *out_applied_count);
/**
 * Provide the editor formatting session cancel operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_formatting_session_cancel(
    UmiEditorFormattingSession *session);
/**
 * Copy editor formatting session edit into module-owned storage so callers keep ownership
 * of their input values.
 */
UmiEditorWorkspaceEditSet *umi_editor_formatting_session_edit_set(
    UmiEditorFormattingSession *session);
/**
 * Provide the editor formatting session snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_formatting_session_snapshot(
    const UmiEditorFormattingSession *session,
    UmiEditorFormattingSessionSnapshot *out_snapshot);
/**
 * Provide the editor formatting session revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_formatting_session_revision(
    const UmiEditorFormattingSession *session);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_FORMATTING_SESSION_H */
