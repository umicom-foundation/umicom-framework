/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/property_commit.c
 *
 * PURPOSE:
 *   Record before/after property values for review, undo and audit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/property_commit.h"
#include <string.h>
UmiStatus umi_rad_property_commit_init(UmiRadPropertyCommit *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->component_id, sizeof item->component_id, "property_commit");
    (void)umi_rad_copy_text(item->property_id, sizeof item->property_id, "property_commit");
    (void)umi_rad_copy_text(item->before_value, sizeof item->before_value, "property_commit");
    (void)umi_rad_copy_text(item->after_value, sizeof item->after_value, "property_commit");
    return UMI_STATUS_OK;
}
int umi_rad_property_commit_is_valid(const UmiRadPropertyCommit *item){if(item==NULL)return 0;return umi_rad_id_valid(item->component_id) && umi_rad_id_valid(item->property_id);}
