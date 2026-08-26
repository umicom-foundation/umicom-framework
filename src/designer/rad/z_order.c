/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/z_order.c
 *
 * PURPOSE:
 *   Represent component stacking order independently of renderer implementation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/z_order.h"
#include <string.h>
UmiStatus umi_rad_z_order_init(UmiRadZOrder *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->component_id, sizeof item->component_id, "z_order");
    return UMI_STATUS_OK;
}
int umi_rad_z_order_is_valid(const UmiRadZOrder *item){if(item==NULL)return 0;return umi_rad_id_valid(item->component_id) && item->order >= 0;}
