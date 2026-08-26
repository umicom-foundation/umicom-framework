/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/drop_target.c
 *
 * PURPOSE:
 *   Represent validated parent/slot destinations during component drag and drop.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/drop_target.h"
#include <string.h>
UmiStatus umi_rad_drop_target_init(UmiRadDropTarget *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->parent_id, sizeof item->parent_id, "drop_target");
    (void)umi_rad_copy_text(item->slot_id, sizeof item->slot_id, "drop_target");
    item->accepted = true;
    return UMI_STATUS_OK;
}
int umi_rad_drop_target_is_valid(const UmiRadDropTarget *item){if(item==NULL)return 0;return umi_rad_id_valid(item->parent_id);}
