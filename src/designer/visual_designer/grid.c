/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/grid.c
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
#include "umicom/designer/visual_designer/grid.h"
#include <string.h>
/*
 * Initialise visual designer grid from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_grid_init(UmiRadDesignGrid *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->spacing_x = 1;
    item->spacing_y = 1;
    item->visible = true;
    return UMI_STATUS_OK;
}
/* Check that visual designer grid satisfies its contract before another service relies on it. */
int umi_rad_grid_is_valid(const UmiRadDesignGrid *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->spacing_x > 0 && item->spacing_y > 0;}
