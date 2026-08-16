/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/snippet_session.h
 *
 * PURPOSE:
 *   Define a provider-neutral snippet parser and traversal session supporting
 *   numbered placeholders, default text, choices and the final cursor stop.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_SNIPPET_SESSION_H
#define UMICOM_EDITOR_SNIPPET_SESSION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_SNIPPET_SESSION_API_VERSION 1U
#define UMI_EDITOR_SNIPPET_ID_CAPACITY 128U
#define UMI_EDITOR_SNIPPET_LANGUAGE_CAPACITY 64U
#define UMI_EDITOR_SNIPPET_NAME_CAPACITY 256U
#define UMI_EDITOR_SNIPPET_BODY_CAPACITY 8192U
#define UMI_EDITOR_SNIPPET_EXPANDED_CAPACITY 16384U
#define UMI_EDITOR_SNIPPET_DEFAULT_CAPACITY 512U
#define UMI_EDITOR_SNIPPET_CHOICES_CAPACITY 1024U

typedef enum UmiEditorSnippetSessionState {
    UMI_EDITOR_SNIPPET_IDLE = 0,
    UMI_EDITOR_SNIPPET_ACTIVE = 1,
    UMI_EDITOR_SNIPPET_COMPLETED = 2,
    UMI_EDITOR_SNIPPET_CANCELLED = 3,
    UMI_EDITOR_SNIPPET_FAILED = 4
} UmiEditorSnippetSessionState;

typedef struct UmiEditorSnippetTemplate {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_SNIPPET_ID_CAPACITY];
    char language_id[UMI_EDITOR_SNIPPET_LANGUAGE_CAPACITY];
    char name[UMI_EDITOR_SNIPPET_NAME_CAPACITY];
    char body[UMI_EDITOR_SNIPPET_BODY_CAPACITY];
} UmiEditorSnippetTemplate;

typedef struct UmiEditorSnippetPlaceholder {
    uint32_t struct_size;
    uint32_t api_version;
    uint32_t ordinal;
    uint64_t start_byte_offset;
    uint64_t end_byte_offset;
    char default_text[UMI_EDITOR_SNIPPET_DEFAULT_CAPACITY];
    char choices[UMI_EDITOR_SNIPPET_CHOICES_CAPACITY];
    int primary;
    int final_stop;
} UmiEditorSnippetPlaceholder;

typedef struct UmiEditorSnippetSessionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char template_id[UMI_EDITOR_SNIPPET_ID_CAPACITY];
    UmiEditorSnippetSessionState state;
    size_t placeholder_count;
    size_t traversal_stop_count;
    size_t active_placeholder_index;
    uint32_t active_ordinal;
    size_t expanded_length;
    uint64_t insertion_byte_offset;
    uint64_t revision;
    int has_active_placeholder;
} UmiEditorSnippetSessionSnapshot;

typedef struct UmiEditorSnippetSession UmiEditorSnippetSession;

UmiStatus umi_editor_snippet_session_create(
    UmiEditorSnippetSession **out_session);
void umi_editor_snippet_session_destroy(UmiEditorSnippetSession *session);
UmiStatus umi_editor_snippet_session_start(
    UmiEditorSnippetSession *session,
    const UmiEditorSnippetTemplate *snippet,
    uint64_t insertion_byte_offset);
UmiStatus umi_editor_snippet_session_next(UmiEditorSnippetSession *session);
UmiStatus umi_editor_snippet_session_previous(UmiEditorSnippetSession *session);
UmiStatus umi_editor_snippet_session_select(
    UmiEditorSnippetSession *session,
    uint32_t ordinal);
UmiStatus umi_editor_snippet_session_cancel(UmiEditorSnippetSession *session);
UmiStatus umi_editor_snippet_session_active(
    const UmiEditorSnippetSession *session,
    UmiEditorSnippetPlaceholder *out_placeholder);
UmiStatus umi_editor_snippet_session_placeholder_at(
    const UmiEditorSnippetSession *session,
    size_t index,
    UmiEditorSnippetPlaceholder *out_placeholder);
UmiStatus umi_editor_snippet_session_expanded_text(
    const UmiEditorSnippetSession *session,
    char *out_text,
    size_t out_capacity);
UmiStatus umi_editor_snippet_session_snapshot(
    const UmiEditorSnippetSession *session,
    UmiEditorSnippetSessionSnapshot *out_snapshot);
size_t umi_editor_snippet_session_placeholder_count(
    const UmiEditorSnippetSession *session);
uint64_t umi_editor_snippet_session_revision(
    const UmiEditorSnippetSession *session);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_SNIPPET_SESSION_H */
