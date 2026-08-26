/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/safe_area_resolver.c
 *
 * PURPOSE:
 *   Apply runtime safe-area semantics to authored designer component rectangles.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/safe_area_resolver.h"

/* Clamp authored geometry to the safe logical viewport while preserving non-negative extents. */
UmiStatus umi_designer_safe_area_resolver_apply(UmiRadRect authored_rect,
                                                UmiAdaptiveViewport viewport,
                                                UmiAdaptiveInsets safe_area,
                                                UmiRadRect *out_rect)
{
    int32_t left,right,top,bottom;
    if(out_rect==NULL||viewport.width<=0||viewport.height<=0)return UMI_STATUS_INVALID_ARGUMENT;
    left=safe_area.left; right=viewport.width-safe_area.right;
    top=safe_area.top; bottom=viewport.height-safe_area.bottom;
    if(left>=right||top>=bottom)return UMI_STATUS_INVALID_STATE;
    *out_rect=authored_rect;
    if(out_rect->x<left)out_rect->x=left;
    if(out_rect->y<top)out_rect->y=top;
    if(out_rect->x+out_rect->width>right)out_rect->width=right-out_rect->x;
    if(out_rect->y+out_rect->height>bottom)out_rect->height=bottom-out_rect->y;
    if(out_rect->width<0)out_rect->width=0;
    if(out_rect->height<0)out_rect->height=0;
    return UMI_STATUS_OK;
}
