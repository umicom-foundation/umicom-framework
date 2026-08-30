/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/hierarchy_node.h
 *
 * PURPOSE:
 *   Represent one node in the designer object hierarchy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_HIERARCHY_NODE_H
#define UMICOM_DESIGNER_RAD_HIERARCHY_NODE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadHierarchyNode {
    char node_id[UMI_RAD_ID_CAPACITY];
    char parent_id[UMI_RAD_ID_CAPACITY];
    int32_t order;
    bool expanded;
} UmiRadHierarchyNode;
UmiStatus umi_rad_hierarchy_node_init(UmiRadHierarchyNode *item);
int umi_rad_hierarchy_node_is_valid(const UmiRadHierarchyNode *item);
#ifdef __cplusplus
}
#endif
#endif
