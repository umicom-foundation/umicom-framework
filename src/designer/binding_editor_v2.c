/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/binding_editor_v2.c
 * PURPOSE: Implement validated state binding editing for Builder v2.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/binding_editor_v2.h"

#include <stdlib.h>
#include <string.h>

struct UmiDesignerBindingEditorV2 {
    UmiDesignerBindingV2 items[UMI_DESIGNER_V2_MAX_BINDINGS];
    size_t count;
    uint64_t revision;
};

static size_t find_index(const UmiDesignerBindingEditorV2 *editor,
                         const char *binding_id)
{
    size_t index;
    if (editor == NULL || binding_id == NULL) return SIZE_MAX;
    for (index = 0U; index < editor->count; ++index) {
        if (strcmp(editor->items[index].binding_id, binding_id) == 0) return index;
    }
    return SIZE_MAX;
}

UmiStatus umi_designer_binding_editor_v2_create(
    UmiDesignerBindingEditorV2 **out_editor)
{
    if (out_editor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_editor = calloc(1U, sizeof(**out_editor));
    if (*out_editor == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (*out_editor)->revision = 1U;
    return UMI_STATUS_OK;
}

void umi_designer_binding_editor_v2_destroy(UmiDesignerBindingEditorV2 *editor)
{
    free(editor);
}

UmiStatus umi_designer_binding_editor_v2_upsert(
    UmiDesignerBindingEditorV2 *editor,
    const UmiDesignerDocument *document,
    const UmiDeclComponentRegistry *components,
    const UmiDesignerBindingV2 *binding)
{
    UmiDeclNode node;
    UmiDeclComponentDescriptor component;
    UmiDeclPropertyDescriptor property;
    size_t index;
    UmiStatus status;
    if (editor == NULL || document == NULL || components == NULL ||
        binding == NULL || binding->binding_id[0] == '\0' ||
        binding->source_expression[0] == '\0' ||
        binding->mode < UMI_DESIGNER_BIND_ONE_WAY ||
        binding->mode > UMI_DESIGNER_BIND_ONE_TIME) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_decl_document_find_node(
        umi_designer_document_declarative((UmiDesignerDocument *)document),
        binding->node_id, &node);
    if (status == UMI_STATUS_OK) status = umi_decl_component_registry_find(
        components, node.component_type, &component);
    if (status == UMI_STATUS_OK) status = umi_decl_component_find_property(
        &component, binding->property_name, &property);
    if (status != UMI_STATUS_OK) return status;
    index = find_index(editor, binding->binding_id);
    if (index == SIZE_MAX) {
        if (editor->count >= UMI_DESIGNER_V2_MAX_BINDINGS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = editor->count++;
    }
    editor->items[index] = *binding;
    editor->revision++;
    editor->items[index].revision = editor->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_designer_binding_editor_v2_remove(
    UmiDesignerBindingEditorV2 *editor,
    const char *binding_id)
{
    size_t index = find_index(editor, binding_id);
    if (editor == NULL || binding_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < editor->count) {
        (void)memmove(&editor->items[index], &editor->items[index + 1U],
                      (editor->count - index - 1U) * sizeof(editor->items[0]));
    }
    editor->count--;
    editor->revision++;
    return UMI_STATUS_OK;
}

UmiStatus umi_designer_binding_editor_v2_find(
    const UmiDesignerBindingEditorV2 *editor,
    const char *binding_id,
    UmiDesignerBindingV2 *out_binding)
{
    size_t index = find_index(editor, binding_id);
    if (editor == NULL || binding_id == NULL || out_binding == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_binding = editor->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_designer_binding_editor_v2_at(
    const UmiDesignerBindingEditorV2 *editor,
    size_t index,
    UmiDesignerBindingV2 *out_binding)
{
    if (editor == NULL || out_binding == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= editor->count) return UMI_STATUS_NOT_FOUND;
    *out_binding = editor->items[index];
    return UMI_STATUS_OK;
}

size_t umi_designer_binding_editor_v2_count(const UmiDesignerBindingEditorV2 *editor)
{
    return editor != NULL ? editor->count : 0U;
}

uint64_t umi_designer_binding_editor_v2_revision(
    const UmiDesignerBindingEditorV2 *editor)
{
    return editor != NULL ? editor->revision : 0U;
}
