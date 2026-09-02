/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/workspace_model.h
 *
 * PURPOSE:
 *   Project one synchronized, toolkit-neutral snapshot for a visual designer's
 *   component palette, hierarchy, selection and property inspector panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A desktop frontend should render this model instead of reading several
 * designer services independently. This keeps all panels on the same document
 * revision and makes selection changes predictable in every Umicom application.
 */
#ifndef UMICOM_DESIGNER_WORKSPACE_MODEL_H
#define UMICOM_DESIGNER_WORKSPACE_MODEL_H

#include "umicom/designer/component_tree.h"
#include "umicom/designer/inspector.h"
#include "umicom/designer/palette.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_WORKSPACE_MODEL_API_VERSION 1U
#define UMI_DESIGNER_WORKSPACE_QUERY_CAPACITY 128U

/**
 * Describe one lightweight component result shown in a visual palette. The
 * full component schema stays in the registry and is not copied into each row.
 */
typedef struct UmiDesignerWorkspacePaletteItem {
    char component_type[UMI_DECL_ID_CAPACITY];
    char display_name[UMI_DECL_NAME_CAPACITY];
    char category[UMI_DECL_NAME_CAPACITY];
    int container;
} UmiDesignerWorkspacePaletteItem;

/**
 * Hold the designer panels that must agree on document revision and selection.
 * Fixed capacities make the snapshot safe to pass across C module boundaries.
 */
typedef struct UmiDesignerWorkspaceModel {
    uint32_t structure_size;
    uint32_t api_version;
    UmiDesignerDocumentSnapshot document;
    UmiDesignerComponentTree hierarchy;
    UmiDesignerInspectorSnapshot inspector;
    UmiDesignerInspectorSchema inspector_schema;
    UmiDesignerWorkspacePaletteItem
        palette_items[UMI_DESIGNER_MAX_PALETTE_RESULTS];
    size_t palette_count;
    char palette_query[UMI_DESIGNER_WORKSPACE_QUERY_CAPACITY];
    char selected_node_id[UMI_DECL_ID_CAPACITY];
    int has_selection;
    int has_inspector;
    int has_inspector_schema;
    uint64_t revision;
} UmiDesignerWorkspaceModel;

/**
 * Build a coherent view of the document for palette, hierarchy and inspector
 * panels. An empty query returns the first bounded group of palette entries.
 */
UmiStatus umi_designer_workspace_model_project(
    const UmiDesignerDocument *document,
    const UmiDesignerSelection *selection,
    const UmiDeclComponentRegistry *components,
    const char *palette_query,
    UmiDesignerWorkspaceModel *out_model);

/**
 * Validate a projected model before a frontend renders it or stores it in a
 * longer-lived view model.
 */
UmiStatus umi_designer_workspace_model_validate(
    const UmiDesignerWorkspaceModel *model);

/**
 * Find a palette row by stable component type without exposing registry-owned
 * storage to the frontend.
 */
const UmiDesignerWorkspacePaletteItem *
umi_designer_workspace_model_find_palette_item(
    const UmiDesignerWorkspaceModel *model,
    const char *component_type);

#ifdef __cplusplus
}
#endif

#endif
