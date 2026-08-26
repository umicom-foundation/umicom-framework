/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/insertion_marker.c
 *
 * PURPOSE:
 *   Represent insertion feedback within ordered containers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/insertion_marker.h"
#include <string.h>
UmiStatus umi_rad_insertion_marker_init(UmiRadInsertionMarker *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->parent_id, sizeof item->parent_id, "insertion_marker");
    item->visible = true;
    return UMI_STATUS_OK;
}
int umi_rad_insertion_marker_is_valid(const UmiRadInsertionMarker *item){if(item==NULL)return 0;return umi_rad_id_valid(item->parent_id) && item->index >= 0;}
