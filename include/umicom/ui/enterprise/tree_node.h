/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/tree_node.h
 *
 * PURPOSE:
 *   Describe a lazily materialised node in an enterprise tree.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_TREE_NODE_H
#define UMICOM_UI_ENTERPRISE_TREE_NODE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiEntTreeNode {
    char node_id[UMI_UI_ENT_ID_CAPACITY];
    char parent_id[UMI_UI_ENT_ID_CAPACITY];
    char label[UMI_UI_ENT_TEXT_CAPACITY];
    int32_t depth;
    int has_children;
    int children_loaded;
    uint64_t revision;
} UmiUiEntTreeNode;
UmiStatus umi_ui_ent_tree_node_init(UmiUiEntTreeNode *value);
int umi_ui_ent_tree_node_validate(const UmiUiEntTreeNode *value);
#ifdef __cplusplus
}
#endif

#endif
