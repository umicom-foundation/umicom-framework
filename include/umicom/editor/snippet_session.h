/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/snippet_session.h
 *
 * PURPOSE:
 *   Define a provider-neutral snippet parser and traversal session supporting
 *   numbered placeholders, default text, choices and the final cursor stop.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * List the named editor snippet session state values accepted by this public contract.
 */
typedef enum UmiEditorSnippetSessionState {
    UMI_EDITOR_SNIPPET_IDLE = 0,
    UMI_EDITOR_SNIPPET_ACTIVE = 1,
    UMI_EDITOR_SNIPPET_COMPLETED = 2,
    UMI_EDITOR_SNIPPET_CANCELLED = 3,
    UMI_EDITOR_SNIPPET_FAILED = 4
} UmiEditorSnippetSessionState;

/**
 * Represent the editor snippet template data shared with callers of this public contract.
 */
typedef struct UmiEditorSnippetTemplate {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_SNIPPET_ID_CAPACITY];
    char language_id[UMI_EDITOR_SNIPPET_LANGUAGE_CAPACITY];
    char name[UMI_EDITOR_SNIPPET_NAME_CAPACITY];
    char body[UMI_EDITOR_SNIPPET_BODY_CAPACITY];
} UmiEditorSnippetTemplate;

/**
 * Represent the editor snippet placeholder data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the editor snippet session snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the editor snippet session data shared with callers of this public contract.
 */
typedef struct UmiEditorSnippetSession UmiEditorSnippetSession;

/**
 * Initialise editor snippet session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_snippet_session_create(
    UmiEditorSnippetSession **out_session);
/**
 * Release or reset state held by editor snippet session so the same storage can be reused
 * safely.
 */
void umi_editor_snippet_session_destroy(UmiEditorSnippetSession *session);
/**
 * Provide the editor snippet session start operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_snippet_session_start(
    UmiEditorSnippetSession *session,
    const UmiEditorSnippetTemplate *snippet,
    uint64_t insertion_byte_offset);
/**
 * Provide the editor snippet session next operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_snippet_session_next(UmiEditorSnippetSession *session);
/**
 * Provide the editor snippet session previous operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_snippet_session_previous(UmiEditorSnippetSession *session);
/**
 * Provide the editor snippet session select operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_snippet_session_select(
    UmiEditorSnippetSession *session,
    uint32_t ordinal);
/**
 * Provide the editor snippet session cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_snippet_session_cancel(UmiEditorSnippetSession *session);
/**
 * Provide the editor snippet session active operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_snippet_session_active(
    const UmiEditorSnippetSession *session,
    UmiEditorSnippetPlaceholder *out_placeholder);
/**
 * Find editor snippet session placeholder while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_snippet_session_placeholder_at(
    const UmiEditorSnippetSession *session,
    size_t index,
    UmiEditorSnippetPlaceholder *out_placeholder);
/**
 * Provide the editor snippet session expanded text operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_snippet_session_expanded_text(
    const UmiEditorSnippetSession *session,
    char *out_text,
    size_t out_capacity);
/**
 * Provide the editor snippet session snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_snippet_session_snapshot(
    const UmiEditorSnippetSession *session,
    UmiEditorSnippetSessionSnapshot *out_snapshot);
/**
 * Return the number of records represented by editor snippet session placeholder without
 * changing their state.
 */
size_t umi_editor_snippet_session_placeholder_count(
    const UmiEditorSnippetSession *session);
/**
 * Provide the editor snippet session revision operation used by this module and its client
 * applications.
 */
uint64_t umi_editor_snippet_session_revision(
    const UmiEditorSnippetSession *session);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_SNIPPET_SESSION_H */
