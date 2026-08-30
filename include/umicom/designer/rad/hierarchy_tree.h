/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/hierarchy_tree.h
 *
 * PURPOSE:
 *   Maintain a bounded component hierarchy with deterministic lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_HIERARCHY_TREE_H
#define UMICOM_DESIGNER_RAD_HIERARCHY_TREE_H
#include "umicom/designer/rad/hierarchy_node.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadHierarchyTree { UmiRadHierarchyNode nodes[UMI_RAD_MAX_ITEMS]; size_t count; uint64_t revision; } UmiRadHierarchyTree;
UmiStatus umi_rad_hierarchy_tree_init(UmiRadHierarchyTree *tree);
UmiStatus umi_rad_hierarchy_tree_add(UmiRadHierarchyTree *tree,const UmiRadHierarchyNode *node);
const UmiRadHierarchyNode *umi_rad_hierarchy_tree_find(const UmiRadHierarchyTree *tree,const char *node_id);
#ifdef __cplusplus
}
#endif
#endif
