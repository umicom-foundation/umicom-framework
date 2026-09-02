/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/drop_target.c
 *
 * PURPOSE:
 *   Represent validated parent/slot destinations during component drag and drop.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/drop_target.h"
#include <string.h>
/*
 * Initialise visual designer drop target from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_drop_target_init(UmiRadDropTarget *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->parent_id, sizeof item->parent_id, "drop_target");
    (void)umi_rad_copy_text(item->slot_id, sizeof item->slot_id, "drop_target");
    item->accepted = true;
    return UMI_STATUS_OK;
}
/* Check that visual designer drop target satisfies its contract before another service relies on it. */
int umi_rad_drop_target_is_valid(const UmiRadDropTarget *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->parent_id);}
