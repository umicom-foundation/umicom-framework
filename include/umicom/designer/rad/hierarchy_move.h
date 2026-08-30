/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/hierarchy_move.h
 *
 * PURPOSE:
 *   Describe a reviewable hierarchy reparent/reorder operation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_HIERARCHY_MOVE_H
#define UMICOM_DESIGNER_RAD_HIERARCHY_MOVE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadHierarchyMove {
    char node_id[UMI_RAD_ID_CAPACITY];
    char old_parent_id[UMI_RAD_ID_CAPACITY];
    char new_parent_id[UMI_RAD_ID_CAPACITY];
    int32_t new_order;
} UmiRadHierarchyMove;
UmiStatus umi_rad_hierarchy_move_init(UmiRadHierarchyMove *item);
int umi_rad_hierarchy_move_is_valid(const UmiRadHierarchyMove *item);
#ifdef __cplusplus
}
#endif
#endif
