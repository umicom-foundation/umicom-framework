/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/validation_marker.c
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
#include "umicom/designer/rad/validation_marker.h"
#include <string.h>
UmiStatus umi_rad_validation_marker_init(UmiRadValidationMarker *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->component_id, sizeof item->component_id, "validation_marker");
    (void)umi_rad_copy_text(item->property_id, sizeof item->property_id, "validation_marker");
    (void)umi_rad_copy_text(item->message, sizeof item->message, "validation_marker");
    return UMI_STATUS_OK;
}
int umi_rad_validation_marker_is_valid(const UmiRadValidationMarker *item){if(item==NULL)return 0;return umi_rad_id_valid(item->component_id) && item->message[0] != '\0';}
