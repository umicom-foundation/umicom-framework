/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/breakpoint_preview.c
 *
 * PURPOSE:
 *   Resolve a named responsive preview breakpoint for the visual canvas.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/breakpoint_preview.h"
#include <string.h>
/*
 * Initialise rad breakpoint preview from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_breakpoint_preview_init(UmiRadBreakpointPreview *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->breakpoint_id, sizeof item->breakpoint_id, "breakpoint_preview");
    item->viewport.width = 1280; item->viewport.height = 720;
    item->dpi = 96U;
    return UMI_STATUS_OK;
}
/*
 * Check that rad breakpoint preview satisfies its contract before another service relies
 * on it.
 */
int umi_rad_breakpoint_preview_is_valid(const UmiRadBreakpointPreview *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->breakpoint_id) && item->viewport.width > 0 && item->viewport.height > 0;}
