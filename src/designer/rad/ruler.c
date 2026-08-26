/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/ruler.c
 *
 * PURPOSE:
 *   Describe design-time rulers and origin offsets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/ruler.h"
#include <string.h>
UmiStatus umi_rad_ruler_init(UmiRadRuler *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->major_step = 1;
    item->visible = true;
    return UMI_STATUS_OK;
}
int umi_rad_ruler_is_valid(const UmiRadRuler *item){if(item==NULL)return 0;return item->major_step > 0;}
