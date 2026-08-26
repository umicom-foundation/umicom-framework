/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/resize_handle.c
 *
 * PURPOSE:
 *   Describe resize-handle semantics without depending on a toolkit cursor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/resize_handle.h"
#include <string.h>
UmiStatus umi_rad_resize_handle_init(UmiRadResizeHandle *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->edges = 1U;
    item->enabled = true;
    return UMI_STATUS_OK;
}
int umi_rad_resize_handle_is_valid(const UmiRadResizeHandle *item){if(item==NULL)return 0;return item->edges != 0U;}
