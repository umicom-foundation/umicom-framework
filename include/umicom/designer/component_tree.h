/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/component_tree.h
 * PURPOSE: Project and edit the semantic component hierarchy for Builder v2.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* The tree is derived from the declarative document, so a tree
 * view never becomes a second source of truth for component relationships. */
#ifndef UMICOM_DESIGNER_COMPONENT_TREE_H
#define UMICOM_DESIGNER_COMPONENT_TREE_H

#include <stdint.h>
#include "umicom/designer/builder_types.h"
#include "umicom/designer/history.h"
#include "umicom/designer/selection.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the designer tree node data shared with callers of this public contract.
 */
typedef struct UmiDesignerTreeNode {
    char node_id[UMI_DECL_ID_CAPACITY];
    char component_type[UMI_DECL_ID_CAPACITY];
    char parent_id[UMI_DECL_ID_CAPACITY];
    uint32_t depth;
    uint32_t sibling_index;
    size_t child_count;
    int selected;
} UmiDesignerTreeNode;

/**
 * Represent the designer component tree data shared with callers of this public contract.
 */
typedef struct UmiDesignerComponentTree {
    UmiDesignerTreeNode nodes[UMI_DESIGNER_MAX_TREE_NODES];
    size_t count;
    uint64_t source_revision;
} UmiDesignerComponentTree;

/**
 * Provide the designer component tree build operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_component_tree_build(
    const UmiDesignerDocument *document,
    const UmiDesignerSelection *selection,
    UmiDesignerComponentTree *out_tree);
/**
 * Find designer component tree while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiDesignerTreeNode *umi_designer_component_tree_find(
    const UmiDesignerComponentTree *tree,
    const char *node_id);
/**
 * Provide the designer component tree reparent operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_component_tree_reparent(
    UmiDesignerHistory *history,
    UmiDesignerDocument *document,
    const char *node_id,
    const char *new_parent_id,
    uint32_t sibling_index);

#ifdef __cplusplus
}
#endif

#endif
