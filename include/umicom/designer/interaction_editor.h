/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/interaction_editor.h
 * PURPOSE: Edit signal, action, event and command mappings for Builder v2.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Interaction mappings contain command identifiers, never raw
 * C function pointers, keeping generated applications inspectable and safe. */
#ifndef UMICOM_DESIGNER_INTERACTION_EDITOR_H
#define UMICOM_DESIGNER_INTERACTION_EDITOR_H

#include "umicom/designer/builder_types.h"
#include "umicom/designer/document.h"

#ifdef __cplusplus
extern "C" {
#endif

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

typedef struct UmiDesignerInteractionEditor UmiDesignerInteractionEditor;

UmiStatus umi_designer_interaction_editor_create(
    UmiDesignerInteractionEditor **out_editor);
void umi_designer_interaction_editor_destroy(
    UmiDesignerInteractionEditor *editor);
UmiStatus umi_designer_interaction_editor_upsert(
    UmiDesignerInteractionEditor *editor,
    const UmiDesignerDocument *document,
    const UmiDesignerInteraction *interaction);
UmiStatus umi_designer_interaction_editor_remove(
    UmiDesignerInteractionEditor *editor,
    const char *interaction_id);
UmiStatus umi_designer_interaction_editor_at(
    const UmiDesignerInteractionEditor *editor,
    size_t index,
    UmiDesignerInteraction *out_interaction);
size_t umi_designer_interaction_editor_count(
    const UmiDesignerInteractionEditor *editor);

#ifdef __cplusplus
}
#endif

#endif
