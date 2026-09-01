/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/binding_editor.h
 * PURPOSE: Manage validated state-to-property bindings for Builder v2.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Bindings are semantic records, not widget callbacks. A
 * frontend edits these records and a runtime adapter later resolves them. */
#ifndef UMICOM_DESIGNER_BINDING_EDITOR_H
#define UMICOM_DESIGNER_BINDING_EDITOR_H

#include "umicom/designer/builder_types.h"
#include "umicom/designer/document.h"
#include "umicom/declarative/component_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerBinding {
    char binding_id[UMI_DECL_ID_CAPACITY];
    char node_id[UMI_DECL_ID_CAPACITY];
    char property_name[UMI_DECL_NAME_CAPACITY];
    char source_expression[UMI_DECL_TEXT_CAPACITY];
    UmiDesignerBindingMode mode;
    int enabled;
    uint64_t revision;
} UmiDesignerBinding;

typedef struct UmiDesignerBindingEditor UmiDesignerBindingEditor;

UmiStatus umi_designer_binding_editor_create(
    UmiDesignerBindingEditor **out_editor);
void umi_designer_binding_editor_destroy(UmiDesignerBindingEditor *editor);
UmiStatus umi_designer_binding_editor_upsert(
    UmiDesignerBindingEditor *editor,
    const UmiDesignerDocument *document,
    const UmiDeclComponentRegistry *components,
    const UmiDesignerBinding *binding);
UmiStatus umi_designer_binding_editor_remove(
    UmiDesignerBindingEditor *editor,
    const char *binding_id);
UmiStatus umi_designer_binding_editor_find(
    const UmiDesignerBindingEditor *editor,
    const char *binding_id,
    UmiDesignerBinding *out_binding);
UmiStatus umi_designer_binding_editor_at(
    const UmiDesignerBindingEditor *editor,
    size_t index,
    UmiDesignerBinding *out_binding);
size_t umi_designer_binding_editor_count(
    const UmiDesignerBindingEditor *editor);
uint64_t umi_designer_binding_editor_revision(
    const UmiDesignerBindingEditor *editor);

#ifdef __cplusplus
}
#endif

#endif
