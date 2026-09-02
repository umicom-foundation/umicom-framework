/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/interaction_editor.h
 * PURPOSE: Edit signal, action, event and command mappings for Builder v2.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Interaction mappings contain command identifiers, never raw
 * C function pointers, keeping generated applications inspectable and safe. */
#ifndef UMICOM_DESIGNER_INTERACTION_EDITOR_H
#define UMICOM_DESIGNER_INTERACTION_EDITOR_H

#include "umicom/designer/builder_types.h"
#include "umicom/designer/document.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the designer interaction data shared with callers of this public contract.
 */
typedef struct UmiDesignerInteraction {
    char interaction_id[UMI_DECL_ID_CAPACITY];
    char node_id[UMI_DECL_ID_CAPACITY];
    UmiDesignerInteractionKind kind;
    char source_name[UMI_DECL_NAME_CAPACITY];
    char command_id[UMI_DECL_ID_CAPACITY];
    char argument[UMI_DECL_TEXT_CAPACITY];
    int enabled;
    uint64_t revision;
} UmiDesignerInteraction;

/**
 * Represent the designer interaction editor data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerInteractionEditor UmiDesignerInteractionEditor;

/**
 * Initialise designer interaction editor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_designer_interaction_editor_create(
    UmiDesignerInteractionEditor **out_editor);
/**
 * Release or reset state held by designer interaction editor so the same storage can be
 * reused safely.
 */
void umi_designer_interaction_editor_destroy(
    UmiDesignerInteractionEditor *editor);
/**
 * Provide the designer interaction editor upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_interaction_editor_upsert(
    UmiDesignerInteractionEditor *editor,
    const UmiDesignerDocument *document,
    const UmiDesignerInteraction *interaction);
/**
 * Remove designer interaction editor while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_designer_interaction_editor_remove(
    UmiDesignerInteractionEditor *editor,
    const char *interaction_id);
/**
 * Find designer interaction editor while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_designer_interaction_editor_at(
    const UmiDesignerInteractionEditor *editor,
    size_t index,
    UmiDesignerInteraction *out_interaction);
/**
 * Return the number of records represented by designer interaction editor without changing
 * their state.
 */
size_t umi_designer_interaction_editor_count(
    const UmiDesignerInteractionEditor *editor);

#ifdef __cplusplus
}
#endif

#endif
