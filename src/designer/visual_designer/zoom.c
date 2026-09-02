/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/zoom.c
 *
 * PURPOSE:
 *   Provide bounded zoom policy for visual authoring surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/zoom.h"
#include <string.h>
/*
 * Initialise visual designer zoom from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_zoom_init(UmiRadZoomPolicy *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->minimum = 0.25;
    item->maximum = 4.0;
    item->current = 4.0;
    return UMI_STATUS_OK;
}
/* Check that visual designer zoom satisfies its contract before another service relies on it. */
int umi_rad_zoom_is_valid(const UmiRadZoomPolicy *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->minimum > 0.0 && item->maximum >= item->minimum && item->current >= item->minimum && item->current <= item->maximum;}
