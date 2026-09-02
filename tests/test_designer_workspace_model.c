/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_designer_workspace_model.c
 *
 * PURPOSE:
 *   Verify palette, hierarchy, selection and inspector panels share one
 *   coherent visual-designer projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/designer/designer.h"

/* Verify every designer panel receives one coherent Framework projection. */
int main(void)
{
    UmiDeclComponentRegistry *components = NULL;
    UmiDeclSchema schema;
    UmiDesignerDocument *document = NULL;
    UmiDesignerSelection selection;
    UmiDesignerWorkspaceModel model;
    UmiDeclNode root;
    UmiDeclNode action;

    (void)memset(&selection, 0, sizeof(selection));
    assert(umi_designer_catalogue_create(
        &components,
        &schema) == UMI_STATUS_OK);
    assert(umi_designer_document_create(
        "org.umicom.example",
        &document) == UMI_STATUS_OK);
    assert(umi_decl_node_init(
        &root,
        "root",
        "window",
        "-") == UMI_STATUS_OK);
    assert(umi_decl_document_add_node(
        umi_designer_document_declarative(document),
        &root) == UMI_STATUS_OK);
    assert(umi_decl_node_init(
        &action,
        "save-action",
        "button",
        "root") == UMI_STATUS_OK);
    assert(umi_decl_node_set_attribute(
        &action,
        "title",
        UMI_DECL_VALUE_STRING,
        "Save") == UMI_STATUS_OK);
    assert(umi_decl_document_add_node(
        umi_designer_document_declarative(document),
        &action) == UMI_STATUS_OK);
    assert(umi_designer_selection_set_primary(
        &selection,
        "save-action") == UMI_STATUS_OK);

    assert(umi_designer_workspace_model_project(
        document,
        &selection,
        components,
        "button",
        &model) == UMI_STATUS_OK);
    assert(umi_designer_workspace_model_validate(&model) == UMI_STATUS_OK);
    assert(model.document.component_count == 2U);
    assert(model.hierarchy.count == 2U);
    assert(model.has_selection);
    assert(model.has_inspector);
    assert(model.has_inspector_schema);
    assert(strcmp(model.selected_node_id, "save-action") == 0);
    assert(model.inspector.attribute_count == 1U);
    assert(model.inspector_schema.property_count > 0U);
    assert(model.palette_count > 0U);
    assert(umi_designer_workspace_model_find_palette_item(
        &model,
        "button") != NULL);

    umi_designer_document_destroy(document);
    umi_decl_component_registry_destroy(components);
    return 0;
}
