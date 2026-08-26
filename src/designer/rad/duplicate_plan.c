/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/duplicate_plan.c
 *
 * PURPOSE:
 *   Describe deterministic component duplication before it is committed.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/duplicate_plan.h"
#include <string.h>
UmiStatus umi_rad_duplicate_plan_init(UmiRadDuplicatePlan *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->source_id, sizeof item->source_id, "duplicate_plan");
    (void)umi_rad_copy_text(item->new_id, sizeof item->new_id, "duplicate_plan");
    (void)umi_rad_copy_text(item->new_parent_id, sizeof item->new_parent_id, "duplicate_plan");
    return UMI_STATUS_OK;
}
int umi_rad_duplicate_plan_is_valid(const UmiRadDuplicatePlan *item){if(item==NULL)return 0;return umi_rad_id_valid(item->source_id) && umi_rad_id_valid(item->new_id);}
