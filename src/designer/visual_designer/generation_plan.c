/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/generation_plan.c
 *
 * PURPOSE:
 *   Describe generated declarative/source artifacts before filesystem writes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/generation_plan.h"
#include <string.h>
/*
 * Initialise visual designer generation plan from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_generation_plan_init(UmiRadGenerationPlan *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->application_id, sizeof item->application_id, "generation_plan");
    (void)umi_rad_copy_text(item->output_root, sizeof item->output_root, "generation_plan");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer generation plan satisfies its contract before another service relies on
 * it.
 */
int umi_rad_generation_plan_is_valid(const UmiRadGenerationPlan *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->application_id) && item->output_root[0] != '\0';}
