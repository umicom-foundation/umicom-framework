/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/preview_state.c
 *
 * PURPOSE:
 *   Record renderer-neutral preview health and diagnostic counts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/preview_state.h"
#include <string.h>
UmiStatus umi_rad_preview_state_init(UmiRadPreviewState *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->document_revision = 0U;
    item->render_revision = 0U;
    item->healthy = true;
    return UMI_STATUS_OK;
}
int umi_rad_preview_state_is_valid(const UmiRadPreviewState *item){if(item==NULL)return 0;return item->render_revision <= item->document_revision;}
