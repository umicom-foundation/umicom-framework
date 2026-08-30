/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/flex_constraint.c
 *
 * PURPOSE:
 *   Describe renderer-neutral flexible-box growth and basis constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/flex_constraint.h"
#include <string.h>
UmiStatus umi_rad_flex_constraint_init(UmiRadFlexConstraint *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->grow = 1.0;
    item->shrink = 1.0;
    item->basis = 1;
    return UMI_STATUS_OK;
}
int umi_rad_flex_constraint_is_valid(const UmiRadFlexConstraint *item){if(item==NULL)return 0;return item->grow >= 0.0 && item->shrink >= 0.0 && item->basis >= 0;}
