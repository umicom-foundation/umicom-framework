/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/hierarchy_move.c
 *
 * PURPOSE:
 *   Describe a reviewable hierarchy reparent/reorder operation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/hierarchy_move.h"
#include <string.h>
UmiStatus umi_rad_hierarchy_move_init(UmiRadHierarchyMove *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->node_id, sizeof item->node_id, "hierarchy_move");
    (void)umi_rad_copy_text(item->old_parent_id, sizeof item->old_parent_id, "hierarchy_move");
    (void)umi_rad_copy_text(item->new_parent_id, sizeof item->new_parent_id, "hierarchy_move");
    return UMI_STATUS_OK;
}
int umi_rad_hierarchy_move_is_valid(const UmiRadHierarchyMove *item){if(item==NULL)return 0;return umi_rad_id_valid(item->node_id) && item->new_order >= 0;}
