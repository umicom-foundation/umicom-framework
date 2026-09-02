/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/drag_session.c
 *
 * PURPOSE:
 *   Track a visual component drag operation from press through commit/cancel.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/drag_session.h"
#include <string.h>
/*
 * Initialise visual designer drag session from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_drag_session_init(UmiRadDragSession *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->component_id, sizeof item->component_id, "drag_session");
    return UMI_STATUS_OK;
}
/* Check that visual designer drag session satisfies its contract before another service relies on it. */
int umi_rad_drag_session_is_valid(const UmiRadDragSession *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->component_id);}
