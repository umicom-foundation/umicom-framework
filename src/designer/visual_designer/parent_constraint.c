/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/parent_constraint.c
 *
 * PURPOSE:
 *   Describe which semantic component families a parent slot accepts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/parent_constraint.h"
#include <string.h>
/*
 * Initialise visual designer parent constraint from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_parent_constraint_init(UmiRadParentConstraint *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->parent_type, sizeof item->parent_type, "parent_constraint");
    (void)umi_rad_copy_text(item->child_family, sizeof item->child_family, "parent_constraint");
    item->accepted = true;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer parent constraint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_parent_constraint_is_valid(const UmiRadParentConstraint *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->parent_type) && umi_rad_id_valid(item->child_family);}
