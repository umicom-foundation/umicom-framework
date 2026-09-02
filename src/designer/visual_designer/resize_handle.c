/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/resize_handle.c
 *
 * PURPOSE:
 *   Describe resize-handle semantics without depending on a toolkit cursor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/resize_handle.h"
#include <string.h>
/*
 * Initialise visual designer resize handle from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_resize_handle_init(UmiRadResizeHandle *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->edges = 1U;
    item->enabled = true;
    return UMI_STATUS_OK;
}
/* Check that visual designer resize handle satisfies its contract before another service relies on it. */
int umi_rad_resize_handle_is_valid(const UmiRadResizeHandle *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->edges != 0U;}
