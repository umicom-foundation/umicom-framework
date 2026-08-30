/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/interaction_editor_v2.h
 * PURPOSE: Edit signal, action, event and command mappings for Builder v2.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Interaction mappings contain command identifiers, never raw
 * C function pointers, keeping generated applications inspectable and safe. */
#ifndef UMICOM_DESIGNER_INTERACTION_EDITOR_V2_H
#define UMICOM_DESIGNER_INTERACTION_EDITOR_V2_H

#include "umicom/designer/builder_types_v2.h"
#include "umicom/designer/document.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerInteractionV2 {
    char interaction_id[UMI_DECL_ID_CAPACITY];
    char node_id[UMI_DECL_ID_CAPACITY];
    UmiDesignerInteractionKindV2 kind;
    char source_name[UMI_DECL_NAME_CAPACITY];
    char command_id[UMI_DECL_ID_CAPACITY];
    char argument[UMI_DECL_TEXT_CAPACITY];
    int enabled;
    uint64_t revision;
} UmiDesignerInteractionV2;

typedef struct UmiDesignerInteractionEditorV2 UmiDesignerInteractionEditorV2;

UmiStatus umi_designer_interaction_editor_v2_create(
    UmiDesignerInteractionEditorV2 **out_editor);
void umi_designer_interaction_editor_v2_destroy(
    UmiDesignerInteractionEditorV2 *editor);
UmiStatus umi_designer_interaction_editor_v2_upsert(
    UmiDesignerInteractionEditorV2 *editor,
    const UmiDesignerDocument *document,
    const UmiDesignerInteractionV2 *interaction);
UmiStatus umi_designer_interaction_editor_v2_remove(
    UmiDesignerInteractionEditorV2 *editor,
    const char *interaction_id);
UmiStatus umi_designer_interaction_editor_v2_at(
    const UmiDesignerInteractionEditorV2 *editor,
    size_t index,
    UmiDesignerInteractionV2 *out_interaction);
size_t umi_designer_interaction_editor_v2_count(
    const UmiDesignerInteractionEditorV2 *editor);

#ifdef __cplusplus
}
#endif

#endif
