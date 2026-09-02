/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_command.h
 *
 * PURPOSE:
 *   Publish canonical editor-intelligence command identities, labels, default
 *   keybindings and enablement requirements for every Umicom application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_COMMAND_H
#define UMICOM_EDITOR_INTELLIGENCE_COMMAND_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_INTELLIGENCE_COMMAND_API_VERSION 1U
#define UMI_EDITOR_INTELLIGENCE_COMMAND_ID_CAPACITY 96U
#define UMI_EDITOR_INTELLIGENCE_COMMAND_LABEL_CAPACITY 128U
#define UMI_EDITOR_INTELLIGENCE_COMMAND_KEY_CAPACITY 48U

/**
 * List the named editor intelligence command kind values accepted by this public contract.
 */
typedef enum UmiEditorIntelligenceCommandKind {
    UMI_EDITOR_INTELLIGENCE_COMMAND_RENAME_SYMBOL = 1,
    UMI_EDITOR_INTELLIGENCE_COMMAND_PREVIEW_RENAME = 2,
    UMI_EDITOR_INTELLIGENCE_COMMAND_APPLY_RENAME = 3,
    UMI_EDITOR_INTELLIGENCE_COMMAND_CANCEL_RENAME = 4,
    UMI_EDITOR_INTELLIGENCE_COMMAND_TOGGLE_CODE_LENS = 5,
    UMI_EDITOR_INTELLIGENCE_COMMAND_REFRESH_CODE_LENS = 6,
    UMI_EDITOR_INTELLIGENCE_COMMAND_EXECUTE_CODE_LENS = 7,
    UMI_EDITOR_INTELLIGENCE_COMMAND_TOGGLE_INLAY_HINTS = 8,
    UMI_EDITOR_INTELLIGENCE_COMMAND_REFRESH_INLAY_HINTS = 9,
    UMI_EDITOR_INTELLIGENCE_COMMAND_TOGGLE_SEMANTIC_HIGHLIGHTING = 10,
    UMI_EDITOR_INTELLIGENCE_COMMAND_REFRESH_SEMANTIC_HIGHLIGHTING = 11,
    UMI_EDITOR_INTELLIGENCE_COMMAND_INSPECT_SEMANTIC_TOKEN = 12
} UmiEditorIntelligenceCommandKind;

/**
 * Represent the editor intelligence command descriptor data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelligenceCommandDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorIntelligenceCommandKind kind;
    char id[UMI_EDITOR_INTELLIGENCE_COMMAND_ID_CAPACITY];
    char label[UMI_EDITOR_INTELLIGENCE_COMMAND_LABEL_CAPACITY];
    char default_key[UMI_EDITOR_INTELLIGENCE_COMMAND_KEY_CAPACITY];
    int requires_symbol;
    int requires_editable_document;
    int opens_view;
} UmiEditorIntelligenceCommandDescriptor;

/**
 * Return the number of records represented by editor intelligence command without changing
 * their state.
 */
size_t umi_editor_intelligence_command_count(void);
/**
 * Find editor intelligence command while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiEditorIntelligenceCommandDescriptor *
umi_editor_intelligence_command_at(size_t index);
/**
 * Find editor intelligence command while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiEditorIntelligenceCommandDescriptor *
umi_editor_intelligence_command_find(const char *id);
/**
 * Provide the editor intelligence command for kind operation used by this module and its
 * client applications.
 */
const UmiEditorIntelligenceCommandDescriptor *
umi_editor_intelligence_command_for_kind(
    UmiEditorIntelligenceCommandKind kind);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_INTELLIGENCE_COMMAND_H */
