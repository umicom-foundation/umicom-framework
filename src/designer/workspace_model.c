/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/workspace_model.c
 *
 * PURPOSE:
 *   Build and validate synchronized visual-designer panel snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/designer/workspace_model.h"

#include <string.h>

/* Copy a registry component into the smaller presentation record. Keeping this
 * conversion here prevents GTK, web and headless frontends from duplicating it. */
static UmiStatus copy_palette_item(
    const UmiDeclComponentDescriptor *source,
    UmiDesignerWorkspacePaletteItem *destination)
{
    UmiStatus status;

    if (source == NULL || destination == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(destination, 0, sizeof(*destination));
    status = umi_decl_copy_text(
        destination->component_type,
        sizeof(destination->component_type),
        source->component_type);
    if (status == UMI_STATUS_OK) {
        status = umi_decl_copy_text(
            destination->display_name,
            sizeof(destination->display_name),
            source->display_name);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_decl_copy_text(
            destination->category,
            sizeof(destination->category),
            source->category);
    }
    if (status == UMI_STATUS_OK) {
        destination->container = source->container != 0;
    }
    return status;
}

/* Copy the palette results while the temporary palette still owns its item
 * array. The finished workspace model never borrows heap-backed palette data. */
static UmiStatus project_palette(
    const UmiDeclComponentRegistry *components,
    const char *query,
    UmiDesignerWorkspaceModel *model)
{
    UmiDesignerPalette palette;
    UmiDesignerPaletteResults results;
    UmiStatus status;
    size_t index;

    (void)memset(&palette, 0, sizeof(palette));
    (void)memset(&results, 0, sizeof(results));
    status = umi_designer_palette_build(components, NULL, &palette);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* A non-empty search uses Framework ranking. An empty search preserves the
     * catalogue order so a new user sees familiar component categories. */
    if (query[0] != '\0') {
        status = umi_designer_palette_search(&palette, query, &results);
    } else {
        results.count = palette.count < UMI_DESIGNER_MAX_PALETTE_RESULTS
            ? palette.count
            : UMI_DESIGNER_MAX_PALETTE_RESULTS;
        for (index = 0U; index < results.count; ++index) {
            results.matches[index].palette_index = index;
            results.matches[index].score = 0;
        }
    }

    /* Stop at the first copy error, but always release the temporary palette
     * before returning so a failed projection cannot leak its item array. */
    for (index = 0U;
         status == UMI_STATUS_OK && index < results.count;
         ++index) {
        const UmiDeclComponentDescriptor *component =
            umi_designer_palette_result(&palette, &results, index);

        if (component == NULL) {
            status = UMI_STATUS_INVALID_STATE;
            break;
        }
        status = copy_palette_item(
            component,
            &model->palette_items[model->palette_count]);
        if (status == UMI_STATUS_OK) {
            model->palette_count += 1U;
        }
    }

    umi_designer_palette_dispose(&palette);
    return status;
}

/* Project all designer panels in a fixed order so later stages only consume
 * state that was successfully produced by earlier stages. */
UmiStatus umi_designer_workspace_model_project(
    const UmiDesignerDocument *document,
    const UmiDesignerSelection *selection,
    const UmiDeclComponentRegistry *components,
    const char *palette_query,
    UmiDesignerWorkspaceModel *out_model)
{
    const char *query = palette_query != NULL ? palette_query : "";
    UmiStatus status;

    if (document == NULL || selection == NULL || components == NULL ||
        out_model == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strlen(query) >= UMI_DESIGNER_WORKSPACE_QUERY_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memset(out_model, 0, sizeof(*out_model));
    out_model->structure_size = (uint32_t)sizeof(*out_model);
    out_model->api_version = UMI_DESIGNER_WORKSPACE_MODEL_API_VERSION;
    status = umi_decl_copy_text(
        out_model->palette_query,
        sizeof(out_model->palette_query),
        query);
    if (status == UMI_STATUS_OK) {
        status = umi_designer_document_snapshot(
            document,
            &out_model->document);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_designer_component_tree_build(
            document,
            selection,
            &out_model->hierarchy);
    }
    if (status == UMI_STATUS_OK) {
        status = project_palette(components, query, out_model);
    }

    /* Selection is optional on a blank design. When it exists, inspection is
     * part of the same projection so the property panel cannot lag the tree. */
    if (status == UMI_STATUS_OK && selection->primary[0] != '\0') {
        status = umi_decl_copy_text(
            out_model->selected_node_id,
            sizeof(out_model->selected_node_id),
            selection->primary);
        if (status == UMI_STATUS_OK) {
            out_model->has_selection = 1;
            status = umi_designer_inspect(
                document,
                selection->primary,
                &out_model->inspector);
        }
        if (status == UMI_STATUS_OK) {
            out_model->has_inspector = 1;
            status = umi_designer_inspector_schema(
                components,
                out_model->inspector.component_type,
                &out_model->inspector_schema);
        }
        if (status == UMI_STATUS_OK) {
            out_model->has_inspector_schema = 1;
        }
    }

    if (status == UMI_STATUS_OK) {
        out_model->revision = out_model->document.revision;
        status = umi_designer_workspace_model_validate(out_model);
    }
    return status;
}

/* Reject structurally inconsistent snapshots before a frontend renders them. */
UmiStatus umi_designer_workspace_model_validate(
    const UmiDesignerWorkspaceModel *model)
{
    if (model == NULL || model->structure_size != sizeof(*model) ||
        model->api_version != UMI_DESIGNER_WORKSPACE_MODEL_API_VERSION ||
        model->palette_count > UMI_DESIGNER_MAX_PALETTE_RESULTS ||
        model->hierarchy.count > UMI_DESIGNER_MAX_TREE_NODES ||
        model->revision != model->document.revision ||
        model->hierarchy.source_revision != model->document.revision ||
        (model->has_inspector && !model->has_selection) ||
        (model->has_inspector_schema && !model->has_inspector) ||
        (model->has_inspector != model->has_inspector_schema) ||
        (model->has_selection && model->selected_node_id[0] == '\0')) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Return a borrowed palette row from the fixed-capacity workspace snapshot. */
const UmiDesignerWorkspacePaletteItem *
umi_designer_workspace_model_find_palette_item(
    const UmiDesignerWorkspaceModel *model,
    const char *component_type)
{
    size_t index;

    if (model == NULL || component_type == NULL) {
        return NULL;
    }
    for (index = 0U; index < model->palette_count; ++index) {
        if (strcmp(
                model->palette_items[index].component_type,
                component_type) == 0) {
            return &model->palette_items[index];
        }
    }
    return NULL;
}
