/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/snap_result.c
 *
 * PURPOSE:
 *   Record the deterministic outcome of a snap calculation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/snap_result.h"
#include <string.h>
/*
 * Initialise rad snap result from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_snap_result_init(UmiRadSnapResult *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);

    return UMI_STATUS_OK;
}
/* Check that rad snap result satisfies its contract before another service relies on it. */
int umi_rad_snap_result_is_valid(const UmiRadSnapResult *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return 1;}
