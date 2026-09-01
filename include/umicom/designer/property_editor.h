/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/property_editor.h
 * PURPOSE: Edit typed component properties with schema validation and undo.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* A draft separates typing and validation from mutation. The
 * document changes only when a valid draft is committed through history. */
#ifndef UMICOM_DESIGNER_PROPERTY_EDITOR_H
#define UMICOM_DESIGNER_PROPERTY_EDITOR_H

#include "umicom/designer/history.h"
#include "umicom/designer/inspector.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerPropertyDraft {
    char node_id[UMI_DECL_ID_CAPACITY];
    char component_type[UMI_DECL_ID_CAPACITY];
    UmiDeclPropertyDescriptor descriptor;
    char original_value[UMI_DECL_TEXT_CAPACITY];
    char value[UMI_DECL_TEXT_CAPACITY];
    UmiStatus validation_status;
    int valid;
    int changed;
} UmiDesignerPropertyDraft;

UmiStatus umi_designer_property_editor_begin(
    const UmiDesignerDocument *document,
    const UmiDeclComponentRegistry *registry,
    const char *node_id,
    const char *property_name,
    UmiDesignerPropertyDraft *out_draft);
UmiStatus umi_designer_property_editor_set(
    UmiDesignerPropertyDraft *draft,
    const char *value_text);
UmiStatus umi_designer_property_editor_commit(
    UmiDesignerHistory *history,
    UmiDesignerDocument *document,
    const UmiDesignerPropertyDraft *draft);

#ifdef __cplusplus
}
#endif

#endif
