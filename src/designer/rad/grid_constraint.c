/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/grid_constraint.c
 *
 * PURPOSE:
 *   Describe renderer-neutral grid row/column placement and spans.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/grid_constraint.h"
#include <string.h>
UmiStatus umi_rad_grid_constraint_init(UmiRadGridConstraint *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->row_span = 1;
    item->column_span = 1;
    return UMI_STATUS_OK;
}
int umi_rad_grid_constraint_is_valid(const UmiRadGridConstraint *item){if(item==NULL)return 0;return item->row >= 0 && item->column >= 0 && item->row_span > 0 && item->column_span > 0;}
