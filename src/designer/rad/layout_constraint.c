/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/layout_constraint.c
 *
 * PURPOSE:
 *   Describe minimum/maximum geometry constraints for designer components.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/layout_constraint.h"
#include <string.h>
/*
 * Initialise rad layout constraint from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_layout_constraint_init(UmiRadLayoutConstraint *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->min_width = 100;
    item->max_width = 100;
    item->min_height = 100;
    item->max_height = 100;
    return UMI_STATUS_OK;
}
/*
 * Check that rad layout constraint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_layout_constraint_is_valid(const UmiRadLayoutConstraint *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->min_width >= 0 && item->min_height >= 0 && item->max_width >= item->min_width && item->max_height >= item->min_height;}
