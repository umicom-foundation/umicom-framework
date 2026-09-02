/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/completion_command.h
 *
 * PURPOSE:
 *   Publish stable command identities for completion lists, provider details,
 *   inline ghost text, partial acceptance and governed AI suggestions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_COMPLETION_COMMAND_H
#define UMICOM_EDITOR_COMPLETION_COMMAND_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_COMPLETION_COMMAND_API_VERSION 1U

/**
 * List the named editor completion command kind values accepted by this public contract.
 */
typedef enum UmiEditorCompletionCommandKind {
    UMI_EDITOR_COMPLETION_COMMAND_TRIGGER = 1,
    UMI_EDITOR_COMPLETION_COMMAND_TRIGGER_SUGGEST = 2,
    UMI_EDITOR_COMPLETION_COMMAND_HIDE = 3,
    UMI_EDITOR_COMPLETION_COMMAND_SELECT_NEXT = 4,
    UMI_EDITOR_COMPLETION_COMMAND_SELECT_PREVIOUS = 5,
    UMI_EDITOR_COMPLETION_COMMAND_SELECT_NEXT_PAGE = 6,
    UMI_EDITOR_COMPLETION_COMMAND_SELECT_PREVIOUS_PAGE = 7,
    UMI_EDITOR_COMPLETION_COMMAND_ACCEPT = 8,
    UMI_EDITOR_COMPLETION_COMMAND_ACCEPT_ENTER = 9,
    UMI_EDITOR_COMPLETION_COMMAND_RESOLVE_DETAILS = 10,
    UMI_EDITOR_COMPLETION_COMMAND_TOGGLE_DETAILS = 11,
    UMI_EDITOR_COMPLETION_COMMAND_FILTER_KIND = 12,
    UMI_EDITOR_COMPLETION_COMMAND_PROVIDER_DIAGNOSTICS = 13,
    UMI_EDITOR_COMPLETION_COMMAND_INLINE_TRIGGER = 14,
    UMI_EDITOR_COMPLETION_COMMAND_INLINE_NEXT = 15,
    UMI_EDITOR_COMPLETION_COMMAND_INLINE_PREVIOUS = 16,
    UMI_EDITOR_COMPLETION_COMMAND_INLINE_ACCEPT = 17,
    UMI_EDITOR_COMPLETION_COMMAND_INLINE_ACCEPT_WORD = 18,
    UMI_EDITOR_COMPLETION_COMMAND_INLINE_ACCEPT_LINE = 19,
    UMI_EDITOR_COMPLETION_COMMAND_INLINE_REJECT = 20,
    UMI_EDITOR_COMPLETION_COMMAND_INLINE_TOGGLE = 21,
    UMI_EDITOR_COMPLETION_COMMAND_INLINE_PAUSE = 22,
    UMI_EDITOR_COMPLETION_COMMAND_AI_ENABLE = 23,
    UMI_EDITOR_COMPLETION_COMMAND_AI_DISABLE = 24
} UmiEditorCompletionCommandKind;

/**
 * Represent the editor completion command descriptor data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorCompletionCommandDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorCompletionCommandKind kind;
    const char *id;
    const char *label;
    const char *default_keybinding;
    int requires_completion_session;
    int requires_completion_candidates;
    int requires_inline_session;
    int mutates_document;
    int requires_confirmation;
} UmiEditorCompletionCommandDescriptor;

/**
 * Return the number of records represented by editor completion command without changing
 * their state.
 */
size_t umi_editor_completion_command_count(void);
/**
 * Find editor completion command while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiEditorCompletionCommandDescriptor *
umi_editor_completion_command_at(size_t position);
/**
 * Find editor completion command while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiEditorCompletionCommandDescriptor *
umi_editor_completion_command_find(const char *command_id);
/**
 * Provide the editor completion command for kind operation used by this module and its
 * client applications.
 */
const UmiEditorCompletionCommandDescriptor *
umi_editor_completion_command_for_kind(UmiEditorCompletionCommandKind kind);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_COMPLETION_COMMAND_H */
