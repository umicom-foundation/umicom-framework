/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/snap_policy.c
 *
 * PURPOSE:
 *   Configure grid, guide and component snapping tolerance.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/snap_policy.h"
#include <string.h>
UmiStatus umi_rad_snap_policy_init(UmiRadSnapPolicy *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->tolerance = 1;
    return UMI_STATUS_OK;
}
int umi_rad_snap_policy_is_valid(const UmiRadSnapPolicy *item){if(item==NULL)return 0;return item->tolerance >= 0;}
