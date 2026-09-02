/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/grid_constraint.c
 *
 * PURPOSE:
 *   Describe renderer-neutral grid row/column placement and spans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/grid_constraint.h"
#include <string.h>
/*
 * Initialise visual designer grid constraint from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_grid_constraint_init(UmiRadGridConstraint *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->row_span = 1;
    item->column_span = 1;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer grid constraint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_grid_constraint_is_valid(const UmiRadGridConstraint *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->row >= 0 && item->column >= 0 && item->row_span > 0 && item->column_span > 0;}
