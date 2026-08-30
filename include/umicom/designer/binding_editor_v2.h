/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/binding_editor_v2.h
 * PURPOSE: Manage validated state-to-property bindings for Builder v2.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Bindings are semantic records, not widget callbacks. A
 * frontend edits these records and a runtime adapter later resolves them. */
#ifndef UMICOM_DESIGNER_BINDING_EDITOR_V2_H
#define UMICOM_DESIGNER_BINDING_EDITOR_V2_H

#include "umicom/designer/builder_types_v2.h"
#include "umicom/designer/document.h"
#include "umicom/declarative/component_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerBindingV2 {
    char binding_id[UMI_DECL_ID_CAPACITY];
    char node_id[UMI_DECL_ID_CAPACITY];
    char property_name[UMI_DECL_NAME_CAPACITY];
    char source_expression[UMI_DECL_TEXT_CAPACITY];
    UmiDesignerBindingModeV2 mode;
    int enabled;
    uint64_t revision;
} UmiDesignerBindingV2;

typedef struct UmiDesignerBindingEditorV2 UmiDesignerBindingEditorV2;

UmiStatus umi_designer_binding_editor_v2_create(
    UmiDesignerBindingEditorV2 **out_editor);
void umi_designer_binding_editor_v2_destroy(UmiDesignerBindingEditorV2 *editor);
UmiStatus umi_designer_binding_editor_v2_upsert(
    UmiDesignerBindingEditorV2 *editor,
    const UmiDesignerDocument *document,
    const UmiDeclComponentRegistry *components,
    const UmiDesignerBindingV2 *binding);
UmiStatus umi_designer_binding_editor_v2_remove(
    UmiDesignerBindingEditorV2 *editor,
    const char *binding_id);
UmiStatus umi_designer_binding_editor_v2_find(
    const UmiDesignerBindingEditorV2 *editor,
    const char *binding_id,
    UmiDesignerBindingV2 *out_binding);
UmiStatus umi_designer_binding_editor_v2_at(
    const UmiDesignerBindingEditorV2 *editor,
    size_t index,
    UmiDesignerBindingV2 *out_binding);
size_t umi_designer_binding_editor_v2_count(
    const UmiDesignerBindingEditorV2 *editor);
uint64_t umi_designer_binding_editor_v2_revision(
    const UmiDesignerBindingEditorV2 *editor);

#ifdef __cplusplus
}
#endif

#endif
