/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/duplicate_plan.c
 *
 * PURPOSE:
 *   Describe deterministic component duplication before it is committed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/duplicate_plan.h"
#include <string.h>
/*
 * Initialise visual designer duplicate plan from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_duplicate_plan_init(UmiRadDuplicatePlan *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->source_id, sizeof item->source_id, "duplicate_plan");
    (void)umi_rad_copy_text(item->new_id, sizeof item->new_id, "duplicate_plan");
    (void)umi_rad_copy_text(item->new_parent_id, sizeof item->new_parent_id, "duplicate_plan");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer duplicate plan satisfies its contract before another service relies on
 * it.
 */
int umi_rad_duplicate_plan_is_valid(const UmiRadDuplicatePlan *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->source_id) && umi_rad_id_valid(item->new_id);}
