/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/property_editor.c
 * PURPOSE: Implement typed, validated and undoable property editing.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/property_editor.h"

#include <string.h>

/*
 * Provide the designer property editor begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_property_editor_begin(
    const UmiDesignerDocument *document,
    const UmiDeclComponentRegistry *registry,
    const char *node_id,
    const char *property_name,
    UmiDesignerPropertyDraft *out_draft)
{
    UmiDeclNode node;
    UmiDeclComponentDescriptor component;
    UmiDeclAttribute attribute;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || registry == NULL || node_id == NULL ||
        property_name == NULL || out_draft == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_draft, 0, sizeof(*out_draft));
    status = umi_decl_document_find_node(
        umi_designer_document_declarative((UmiDesignerDocument *)document),
        node_id, &node);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_decl_component_registry_find(
        registry, node.component_type, &component);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_decl_component_find_property(
        &component, property_name, &out_draft->descriptor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)umi_decl_copy_text(out_draft->node_id, sizeof(out_draft->node_id),
                             node.node_id);
    (void)umi_decl_copy_text(out_draft->component_type,
                             sizeof(out_draft->component_type),
                             node.component_type);
    status = umi_decl_node_get_attribute(&node, property_name, &attribute);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_decl_value_as_text(&attribute.value,
                                        out_draft->original_value,
                                        sizeof(out_draft->original_value));
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_NOT_FOUND) {
        (void)umi_decl_copy_text(out_draft->original_value,
                                 sizeof(out_draft->original_value),
                                 out_draft->descriptor.default_value);
        status = UMI_STATUS_OK;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        (void)umi_decl_copy_text(out_draft->value, sizeof(out_draft->value),
                                 out_draft->original_value);
        out_draft->validation_status = UMI_STATUS_OK;
        out_draft->valid = 1;
    }
    return status;
}

/*
 * Copy designer property editor into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_designer_property_editor_set(
    UmiDesignerPropertyDraft *draft,
    const char *value_text)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (draft == NULL || value_text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_decl_property_validate_text(&draft->descriptor, value_text);
    draft->validation_status = status;
    draft->valid = status == UMI_STATUS_OK;
    draft->changed = strcmp(draft->original_value, value_text) != 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_decl_copy_text(draft->value, sizeof(draft->value), value_text)
        != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    return status;
}

/*
 * Provide the designer property editor commit operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_property_editor_commit(
    UmiDesignerHistory *history,
    UmiDesignerDocument *document,
    const UmiDesignerPropertyDraft *draft)
{
    UmiDeclNode before;
    UmiDeclNode after;
    UmiDesignerOperation operation;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || document == NULL || draft == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!draft->valid) return UMI_STATUS_PARSE_ERROR;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!draft->changed) return UMI_STATUS_OK;
    status = umi_decl_document_find_node(
        umi_designer_document_declarative(document), draft->node_id, &before);
    after = before;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_decl_node_set_attribute(
        &after, draft->descriptor.name, draft->descriptor.kind, draft->value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_operation_set_property(
        &before, &after, draft->descriptor.name, &operation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_designer_history_execute(
        history, &operation);
    return status;
}
