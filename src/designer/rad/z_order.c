/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/z_order.c
 *
 * PURPOSE:
 *   Represent component stacking order independently of renderer implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/z_order.h"
#include <string.h>
/*
 * Initialise rad z order from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_z_order_init(UmiRadZOrder *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->component_id, sizeof item->component_id, "z_order");
    return UMI_STATUS_OK;
}
/* Check that rad z order satisfies its contract before another service relies on it. */
int umi_rad_z_order_is_valid(const UmiRadZOrder *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->component_id) && item->order >= 0;}
