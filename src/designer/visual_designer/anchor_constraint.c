/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/anchor_constraint.c
 *
 * PURPOSE:
 *   Describe edge anchors for adaptive layouts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/anchor_constraint.h"
#include <string.h>
/*
 * Initialise visual designer anchor constraint from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_anchor_constraint_init(UmiRadAnchorConstraint *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->margin = 1;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer anchor constraint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_anchor_constraint_is_valid(const UmiRadAnchorConstraint *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->margin >= 0;}
