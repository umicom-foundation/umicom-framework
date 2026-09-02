/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/pan.c
 *
 * PURPOSE:
 *   Provide deterministic canvas panning state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/pan.h"
#include <string.h>
/*
 * Initialise visual designer pan from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_pan_init(UmiRadPanState *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);

    return UMI_STATUS_OK;
}
/* Check that visual designer pan satisfies its contract before another service relies on it. */
int umi_rad_pan_is_valid(const UmiRadPanState *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return 1;}
