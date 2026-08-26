/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/drag_session.c
 *
 * PURPOSE:
 *   Track a visual component drag operation from press through commit/cancel.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/drag_session.h"
#include <string.h>
UmiStatus umi_rad_drag_session_init(UmiRadDragSession *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->component_id, sizeof item->component_id, "drag_session");
    return UMI_STATUS_OK;
}
int umi_rad_drag_session_is_valid(const UmiRadDragSession *item){if(item==NULL)return 0;return umi_rad_id_valid(item->component_id);}
