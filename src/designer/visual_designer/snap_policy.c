/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/snap_policy.c
 *
 * PURPOSE:
 *   Configure grid, guide and component snapping tolerance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/snap_policy.h"
#include <string.h>
/*
 * Initialise visual designer snap policy from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_snap_policy_init(UmiRadSnapPolicy *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->tolerance = 1;
    return UMI_STATUS_OK;
}
/* Check that visual designer snap policy satisfies its contract before another service relies on it. */
int umi_rad_snap_policy_is_valid(const UmiRadSnapPolicy *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->tolerance >= 0;}
