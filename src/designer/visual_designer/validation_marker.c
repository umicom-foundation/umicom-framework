/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/validation_marker.c
 *
 * PURPOSE:
 *   Attach a validation severity/message to a component or property.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/validation_marker.h"
#include <string.h>
/*
 * Initialise visual designer validation marker from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_validation_marker_init(UmiRadValidationMarker *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->component_id, sizeof item->component_id, "validation_marker");
    (void)umi_rad_copy_text(item->property_id, sizeof item->property_id, "validation_marker");
    (void)umi_rad_copy_text(item->message, sizeof item->message, "validation_marker");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer validation marker satisfies its contract before another service relies on
 * it.
 */
int umi_rad_validation_marker_is_valid(const UmiRadValidationMarker *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->component_id) && item->message[0] != '\0';}
