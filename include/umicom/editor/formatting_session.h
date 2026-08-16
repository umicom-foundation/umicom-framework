/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/formatting_session.h
 *
 * PURPOSE:
 *   Coordinate revision-safe document, range, paste, save and on-type
 *   formatting operations through canonical Framework workspace edits.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef enum UmiEditorFormattingRequestMode {
    UMI_EDITOR_FORMATTING_DOCUMENT = 1,
    UMI_EDITOR_FORMATTING_RANGE = 2,
    UMI_EDITOR_FORMATTING_ON_TYPE = 3,
    UMI_EDITOR_FORMATTING_ON_SAVE = 4,
    UMI_EDITOR_FORMATTING_ON_PASTE = 5
} UmiEditorFormattingRequestMode;

typedef enum UmiEditorFormattingSessionState {
    UMI_EDITOR_FORMATTING_IDLE = 0,
    UMI_EDITOR_FORMATTING_COLLECTING = 1,
    UMI_EDITOR_FORMATTING_READY = 2,
    UMI_EDITOR_FORMATTING_APPLIED = 3,
    UMI_EDITOR_FORMATTING_CONFLICT = 4,
    UMI_EDITOR_FORMATTING_CANCELLED = 5,
    UMI_EDITOR_FORMATTING_FAILED = 6
} UmiEditorFormattingSessionState;

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

typedef struct UmiEditorFormattingSession UmiEditorFormattingSession;

UmiStatus umi_editor_formatting_session_create(
    UmiEditorFormattingSession **out_session);
void umi_editor_formatting_session_destroy(UmiEditorFormattingSession *session);
UmiStatus umi_editor_formatting_session_begin(
    UmiEditorFormattingSession *session,
    const UmiEditorFormattingRequest *request);
UmiStatus umi_editor_formatting_session_add_edit(
    UmiEditorFormattingSession *session,
    const UmiEditorWorkspaceTextEdit *edit);
UmiStatus umi_editor_formatting_session_finalize(
    UmiEditorFormattingSession *session);
UmiStatus umi_editor_formatting_session_apply(
    UmiEditorFormattingSession *session,
    UmiEditorTextBuffer *buffer,
    size_t *out_applied_count);
UmiStatus umi_editor_formatting_session_cancel(
    UmiEditorFormattingSession *session);
UmiEditorWorkspaceEditSet *umi_editor_formatting_session_edit_set(
    UmiEditorFormattingSession *session);
UmiStatus umi_editor_formatting_session_snapshot(
    const UmiEditorFormattingSession *session,
    UmiEditorFormattingSessionSnapshot *out_snapshot);
uint64_t umi_editor_formatting_session_revision(
    const UmiEditorFormattingSession *session);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_FORMATTING_SESSION_H */
