/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/safe_area_resolver.c
 *
 * PURPOSE:
 *   Apply runtime safe-area semantics to authored designer component rectangles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/safe_area_resolver.h"

/* Clamp authored geometry to the safe logical viewport while preserving non-negative extents. */
UmiStatus umi_designer_safe_area_resolver_apply(UmiRadRect authored_rect,
                                                UmiAdaptiveViewport viewport,
                                                UmiAdaptiveInsets safe_area,
                                                UmiRadRect *out_rect)
{
    int32_t left,right,top,bottom;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(out_rect==NULL||viewport.width<=0||viewport.height<=0)return UMI_STATUS_INVALID_ARGUMENT;
    left=safe_area.left; right=viewport.width-safe_area.right;
    top=safe_area.top; bottom=viewport.height-safe_area.bottom;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(left>=right||top>=bottom)return UMI_STATUS_INVALID_STATE;
    *out_rect=authored_rect;
    /* Apply this branch only when its contract condition is satisfied. */
    if(out_rect->x<left)out_rect->x=left;
    /* Apply this branch only when its contract condition is satisfied. */
    if(out_rect->y<top)out_rect->y=top;
    /* Apply this branch only when its contract condition is satisfied. */
    if(out_rect->x+out_rect->width>right)out_rect->width=right-out_rect->x;
    /* Apply this branch only when its contract condition is satisfied. */
    if(out_rect->y+out_rect->height>bottom)out_rect->height=bottom-out_rect->y;
    /* Apply this branch only when its contract condition is satisfied. */
    if(out_rect->width<0)out_rect->width=0;
    /* Apply this branch only when its contract condition is satisfied. */
    if(out_rect->height<0)out_rect->height=0;
    return UMI_STATUS_OK;
}
