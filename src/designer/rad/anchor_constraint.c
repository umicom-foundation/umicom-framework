/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/anchor_constraint.c
 *
 * PURPOSE:
 *   Describe edge anchors for adaptive layouts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/anchor_constraint.h"
#include <string.h>
UmiStatus umi_rad_anchor_constraint_init(UmiRadAnchorConstraint *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->margin = 1;
    return UMI_STATUS_OK;
}
int umi_rad_anchor_constraint_is_valid(const UmiRadAnchorConstraint *item){if(item==NULL)return 0;return item->margin >= 0;}
