/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/snap_result.c
 *
 * PURPOSE:
 *   Record the deterministic outcome of a snap calculation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/snap_result.h"
#include <string.h>
UmiStatus umi_rad_snap_result_init(UmiRadSnapResult *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);

    return UMI_STATUS_OK;
}
int umi_rad_snap_result_is_valid(const UmiRadSnapResult *item){if(item==NULL)return 0;return 1;}
