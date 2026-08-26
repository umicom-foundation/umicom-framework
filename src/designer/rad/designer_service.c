/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/designer_service.c
 *
 * PURPOSE:
 *   Aggregate RAD designer readiness and active-session state for thin frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/designer_service.h"
#include <string.h>
UmiStatus umi_rad_designer_service_init(UmiRadDesignerService *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    item->conformance_score = 100U;
    item->initialized = true;
    return UMI_STATUS_OK;
}
int umi_rad_designer_service_is_valid(const UmiRadDesignerService *item){if(item==NULL)return 0;return item->conformance_score <= 100U;}
