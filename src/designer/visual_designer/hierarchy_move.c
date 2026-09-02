/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/hierarchy_move.c
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
#include "umicom/designer/visual_designer/hierarchy_move.h"
#include <string.h>
/*
 * Initialise visual designer hierarchy move from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_hierarchy_move_init(UmiRadHierarchyMove *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->node_id, sizeof item->node_id, "hierarchy_move");
    (void)umi_rad_copy_text(item->old_parent_id, sizeof item->old_parent_id, "hierarchy_move");
    (void)umi_rad_copy_text(item->new_parent_id, sizeof item->new_parent_id, "hierarchy_move");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer hierarchy move satisfies its contract before another service relies on
 * it.
 */
int umi_rad_hierarchy_move_is_valid(const UmiRadHierarchyMove *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->node_id) && item->new_order >= 0;}
