/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/grid.c
 *
 * PURPOSE:
 *   Describe configurable design-time grid spacing and visibility.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/grid.h"
#include <string.h>
UmiStatus umi_rad_grid_init(UmiRadDesignGrid *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->spacing_x = 1;
    item->spacing_y = 1;
    item->visible = true;
    return UMI_STATUS_OK;
}
int umi_rad_grid_is_valid(const UmiRadDesignGrid *item){if(item==NULL)return 0;return item->spacing_x > 0 && item->spacing_y > 0;}
