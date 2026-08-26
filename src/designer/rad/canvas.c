/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/canvas.c
 *
 * PURPOSE:
 *   Describe a visual application design canvas and its revision state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/canvas.h"
#include <string.h>
UmiStatus umi_rad_canvas_init(UmiRadCanvas *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->document_id, sizeof item->document_id, "canvas");
    (void)umi_rad_copy_text(item->root_component_id, sizeof item->root_component_id, "canvas");
    return UMI_STATUS_OK;
}
int umi_rad_canvas_is_valid(const UmiRadCanvas *item){if(item==NULL)return 0;return umi_rad_id_valid(item->root_component_id);}
