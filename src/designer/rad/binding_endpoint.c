/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/binding_endpoint.c
 *
 * PURPOSE:
 *   Represent one source or destination property endpoint in the visual binding editor.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/binding_endpoint.h"
#include <string.h>
UmiStatus umi_rad_binding_endpoint_init(UmiRadBindingEndpoint *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->node_id, sizeof item->node_id, "binding_endpoint");
    (void)umi_rad_copy_text(item->property_path, sizeof item->property_path, "binding_endpoint");
    return UMI_STATUS_OK;
}
int umi_rad_binding_endpoint_is_valid(const UmiRadBindingEndpoint *item){if(item==NULL)return 0;return umi_rad_id_valid(item->node_id) && item->property_path[0] != '\0';}
