/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/generation_plan.c
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
#include "umicom/designer/rad/generation_plan.h"
#include <string.h>
UmiStatus umi_rad_generation_plan_init(UmiRadGenerationPlan *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->application_id, sizeof item->application_id, "generation_plan");
    (void)umi_rad_copy_text(item->output_root, sizeof item->output_root, "generation_plan");
    return UMI_STATUS_OK;
}
int umi_rad_generation_plan_is_valid(const UmiRadGenerationPlan *item){if(item==NULL)return 0;return umi_rad_id_valid(item->application_id) && item->output_root[0] != '\0';}
