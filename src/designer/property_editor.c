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
    if (document == NULL || registry == NULL || node_id == NULL ||
        property_name == NULL || out_draft == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_draft, 0, sizeof(*out_draft));
    status = umi_decl_document_find_node(
        umi_designer_document_declarative((UmiDesignerDocument *)document),
        node_id, &node);
    if (status == UMI_STATUS_OK) status = umi_decl_component_registry_find(
        registry, node.component_type, &component);
    if (status == UMI_STATUS_OK) status = umi_decl_component_find_property(
        &component, property_name, &out_draft->descriptor);
    if (status != UMI_STATUS_OK) return status;
    (void)umi_decl_copy_text(out_draft->node_id, sizeof(out_draft->node_id),
                             node.node_id);
    (void)umi_decl_copy_text(out_draft->component_type,
                             sizeof(out_draft->component_type),
                             node.component_type);
    status = umi_decl_node_get_attribute(&node, property_name, &attribute);
    if (status == UMI_STATUS_OK) {
        status = umi_decl_value_as_text(&attribute.value,
                                        out_draft->original_value,
                                        sizeof(out_draft->original_value));
    } else if (status == UMI_STATUS_NOT_FOUND) {
        (void)umi_decl_copy_text(out_draft->original_value,
                                 sizeof(out_draft->original_value),
                                 out_draft->descriptor.default_value);
        status = UMI_STATUS_OK;
    }
    if (status == UMI_STATUS_OK) {
        (void)umi_decl_copy_text(out_draft->value, sizeof(out_draft->value),
                                 out_draft->original_value);
        out_draft->validation_status = UMI_STATUS_OK;
        out_draft->valid = 1;
    }
    return status;
}

UmiStatus umi_designer_property_editor_set(
    UmiDesignerPropertyDraft *draft,
    const char *value_text)
{
    UmiStatus status;
    if (draft == NULL || value_text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_decl_property_validate_text(&draft->descriptor, value_text);
    draft->validation_status = status;
    draft->valid = status == UMI_STATUS_OK;
    draft->changed = strcmp(draft->original_value, value_text) != 0;
    if (umi_decl_copy_text(draft->value, sizeof(draft->value), value_text)
        != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    return status;
}

UmiStatus umi_designer_property_editor_commit(
    UmiDesignerHistory *history,
    UmiDesignerDocument *document,
    const UmiDesignerPropertyDraft *draft)
{
    UmiDeclNode before;
    UmiDeclNode after;
    UmiDesignerOperation operation;
    UmiStatus status;
    if (history == NULL || document == NULL || draft == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!draft->valid) return UMI_STATUS_PARSE_ERROR;
    if (!draft->changed) return UMI_STATUS_OK;
    status = umi_decl_document_find_node(
        umi_designer_document_declarative(document), draft->node_id, &before);
    after = before;
    if (status == UMI_STATUS_OK) status = umi_decl_node_set_attribute(
        &after, draft->descriptor.name, draft->descriptor.kind, draft->value);
    if (status == UMI_STATUS_OK) status = umi_designer_operation_set_property(
        &before, &after, draft->descriptor.name, &operation);
    if (status == UMI_STATUS_OK) status = umi_designer_history_execute(
        history, &operation);
    return status;
}
