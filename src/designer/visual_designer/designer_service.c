/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/designer_service.c
 *
 * PURPOSE:
 *   Aggregate visual designer readiness and active-session state for thin frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/designer_service.h"
#include <string.h>
/*
 * Initialise visual designer service from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_designer_service_init(UmiRadDesignerService *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->conformance_score = 100U;
    item->initialized = true;
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer service satisfies its contract before another service relies on
 * it.
 */
int umi_rad_designer_service_is_valid(const UmiRadDesignerService *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return item->conformance_score <= 100U;}
