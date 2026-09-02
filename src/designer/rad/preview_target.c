/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/preview_target.c
 *
 * PURPOSE:
 *   Describe GTK4, Qt6, Native Web or abstract-device preview targets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/preview_target.h"
#include <string.h>
/*
 * Initialise rad preview target from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_preview_target_init(UmiRadPreviewTarget *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->target_id, sizeof item->target_id, "preview_target");
    item->viewport.width = 1280; item->viewport.height = 720;
    item->dpi = 96U;
    return UMI_STATUS_OK;
}
/*
 * Check that rad preview target satisfies its contract before another service relies on
 * it.
 */
int umi_rad_preview_target_is_valid(const UmiRadPreviewTarget *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->target_id) && item->viewport.width > 0 && item->viewport.height > 0;}
