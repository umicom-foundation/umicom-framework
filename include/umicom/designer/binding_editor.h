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

/**
 * Represent the designer binding data shared with callers of this public contract.
 */
typedef struct UmiDesignerBinding {
    char binding_id[UMI_DECL_ID_CAPACITY];
    char node_id[UMI_DECL_ID_CAPACITY];
    char property_name[UMI_DECL_NAME_CAPACITY];
    char source_expression[UMI_DECL_TEXT_CAPACITY];
    UmiDesignerBindingMode mode;
    int enabled;
    uint64_t revision;
} UmiDesignerBinding;

/**
 * Represent the designer binding editor data shared with callers of this public contract.
 */
typedef struct UmiDesignerBindingEditor UmiDesignerBindingEditor;

/**
 * Initialise designer binding editor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_designer_binding_editor_create(
    UmiDesignerBindingEditor **out_editor);
/**
 * Release or reset state held by designer binding editor so the same storage can be reused
 * safely.
 */
void umi_designer_binding_editor_destroy(UmiDesignerBindingEditor *editor);
/**
 * Provide the designer binding editor upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_binding_editor_upsert(
    UmiDesignerBindingEditor *editor,
    const UmiDesignerDocument *document,
    const UmiDeclComponentRegistry *components,
    const UmiDesignerBinding *binding);
/**
 * Remove designer binding editor while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_designer_binding_editor_remove(
    UmiDesignerBindingEditor *editor,
    const char *binding_id);
/**
 * Find designer binding editor while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_designer_binding_editor_find(
    const UmiDesignerBindingEditor *editor,
    const char *binding_id,
    UmiDesignerBinding *out_binding);
/**
 * Find designer binding editor while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_designer_binding_editor_at(
    const UmiDesignerBindingEditor *editor,
    size_t index,
    UmiDesignerBinding *out_binding);
/**
 * Return the number of records represented by designer binding editor without changing
 * their state.
 */
size_t umi_designer_binding_editor_count(
    const UmiDesignerBindingEditor *editor);
/**
 * Provide the designer binding editor revision operation used by this module and its
 * client applications.
 */
uint64_t umi_designer_binding_editor_revision(
    const UmiDesignerBindingEditor *editor);

#ifdef __cplusplus
}
#endif

#endif
