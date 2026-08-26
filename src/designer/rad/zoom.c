/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/zoom.c
 *
 * PURPOSE:
 *   Provide bounded zoom policy for visual authoring surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/zoom.h"
#include <string.h>
UmiStatus umi_rad_zoom_init(UmiRadZoomPolicy *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->minimum = 0.25;
    item->maximum = 4.0;
    item->current = 4.0;
    return UMI_STATUS_OK;
}
int umi_rad_zoom_is_valid(const UmiRadZoomPolicy *item){if(item==NULL)return 0;return item->minimum > 0.0 && item->maximum >= item->minimum && item->current >= item->minimum && item->current <= item->maximum;}
