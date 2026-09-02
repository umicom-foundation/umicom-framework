/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/distribution_guide.c
 *
 * PURPOSE:
 *   Represent equal-spacing evidence for multiple selected components.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/distribution_guide.h"
#include <string.h>
/*
 * Initialise visual designer distribution guide from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_distribution_guide_init(UmiRadDistributionGuide *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->group_id, sizeof item->group_id, "distribution_guide");
    item->spacing = 1;
    item->item_count = 2U;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer distribution guide satisfies its contract before another service relies
 * on it.
 */
int umi_rad_distribution_guide_is_valid(const UmiRadDistributionGuide *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->group_id) && item->spacing >= 0 && item->item_count >= 2U;}
