/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/ruler.c
 *
 * PURPOSE:
 *   Describe design-time rulers and origin offsets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/ruler.h"
#include <string.h>
/*
 * Initialise visual designer ruler from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_ruler_init(UmiRadRuler *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->major_step = 1;
    item->visible = true;
    return UMI_STATUS_OK;
}
/* Check that visual designer ruler satisfies its contract before another service relies on it. */
int umi_rad_ruler_is_valid(const UmiRadRuler *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->major_step > 0;}
