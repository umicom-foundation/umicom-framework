/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/tree.h
 *
 * PURPOSE:
 *   Project the semantic layout hierarchy into expandable rows for a component
 *   tree without exposing widget state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_TREE_H
#define UMICOM_WORKBENCH_DESIGNER_TREE_H

#include "umicom/workbench_designer/selection.h"
#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer tree row data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerTreeRow {
    char node_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char title[UMI_WORKBENCH_DESIGNER_LABEL_CAPACITY];
    char component_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchLayoutNodeKind kind;
    size_t node_index;
    size_t depth;
    size_t child_count;
    bool expanded;
    bool selected;
    bool primary;
    bool visible;
} UmiWorkbenchDesignerTreeRow;

/**
 * Represent the workbench designer tree data shared with callers of this public contract.
 */
typedef struct UmiWorkbenchDesignerTree {
    UmiWorkbenchDesignerTreeRow rows[UMI_WORKBENCH_DESIGNER_MAX_TREE_ROWS];
    size_t row_count;
    UmiWorkbenchDesignerIdentifier collapsed_nodes[UMI_WORKBENCH_DESIGNER_MAX_TREE_ROWS];
    size_t collapsed_count;
    uint64_t revision;
} UmiWorkbenchDesignerTree;

/**
 * Initialise workbench designer tree from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_tree_init(UmiWorkbenchDesignerTree *tree);
/**
 * Provide the workbench designer tree build operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_tree_build(UmiWorkbenchDesignerTree *tree, const UmiWorkbenchLayoutDocument *document, const UmiWorkbenchDesignerSelection *selection);
/**
 * Provide the workbench designer tree set expanded operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_tree_set_expanded(UmiWorkbenchDesignerTree *tree, const char *node_id, bool expanded);
/**
 * Provide the workbench designer tree is expanded operation used by this module and its
 * client applications.
 */
bool umi_workbench_designer_tree_is_expanded(const UmiWorkbenchDesignerTree *tree, const char *node_id);
/**
 * Find workbench designer tree while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchDesignerTreeRow *umi_workbench_designer_tree_at(const UmiWorkbenchDesignerTree *tree, size_t index);
/**
 * Find workbench designer tree while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchDesignerTreeRow *umi_workbench_designer_tree_find(const UmiWorkbenchDesignerTree *tree, const char *node_id);

#ifdef __cplusplus
}
#endif

#endif
