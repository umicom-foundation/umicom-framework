/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/guide.c
 *
 * PURPOSE:
 *   Represent user-created horizontal and vertical design guides.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/guide.h"
#include <string.h>
/*
 * Initialise rad guide from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_rad_guide_init(UmiRadGuide *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->guide_id, sizeof item->guide_id, "guide");
    return UMI_STATUS_OK;
}
/* Check that rad guide satisfies its contract before another service relies on it. */
int umi_rad_guide_is_valid(const UmiRadGuide *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->guide_id);}
