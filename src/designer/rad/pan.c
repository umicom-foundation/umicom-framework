/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/pan.c
 *
 * PURPOSE:
 *   Provide deterministic canvas panning state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/pan.h"
#include <string.h>
UmiStatus umi_rad_pan_init(UmiRadPanState *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);

    return UMI_STATUS_OK;
}
int umi_rad_pan_is_valid(const UmiRadPanState *item){if(item==NULL)return 0;return 1;}
