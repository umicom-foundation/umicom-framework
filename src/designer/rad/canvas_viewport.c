/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/canvas_viewport.c
 *
 * PURPOSE:
 *   Track canvas origin, dimensions and zoom independently from document geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/canvas_viewport.h"
#include <string.h>
/*
 * Initialise rad canvas viewport from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_canvas_viewport_init(UmiRadCanvasViewport *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->extent.width = 1280; item->extent.height = 720;
    item->zoom = 1.0;
    return UMI_STATUS_OK;
}
/*
 * Check that rad canvas viewport satisfies its contract before another service relies on
 * it.
 */
int umi_rad_canvas_viewport_is_valid(const UmiRadCanvasViewport *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->extent.width > 0 && item->extent.height > 0 && item->zoom > 0.0;}
