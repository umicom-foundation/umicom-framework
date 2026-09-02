/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/preview_state.c
 *
 * PURPOSE:
 *   Record renderer-neutral preview health and diagnostic counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/preview_state.h"
#include <string.h>
/*
 * Initialise visual designer preview state from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_preview_state_init(UmiRadPreviewState *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->document_revision = 0U;
    item->render_revision = 0U;
    item->healthy = true;
    return UMI_STATUS_OK;
}
/* Check that visual designer preview state satisfies its contract before another service relies on it. */
int umi_rad_preview_state_is_valid(const UmiRadPreviewState *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->render_revision <= item->document_revision;}
