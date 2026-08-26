/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/component_catalogue_bridge.c
 *
 * PURPOSE:
 *   Map Design System component identifiers to canonical designer component types.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/component_catalogue_bridge.h"
#include <string.h>
UmiStatus umi_rad_component_catalogue_bridge_init(UmiRadComponentCatalogueBridge *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->design_component_id, sizeof item->design_component_id, "component_catalogue_bridge");
    (void)umi_rad_copy_text(item->designer_type, sizeof item->designer_type, "component_catalogue_bridge");
    (void)umi_rad_copy_text(item->family, sizeof item->family, "component_catalogue_bridge");
    return UMI_STATUS_OK;
}
int umi_rad_component_catalogue_bridge_is_valid(const UmiRadComponentCatalogueBridge *item){if(item==NULL)return 0;return umi_rad_id_valid(item->design_component_id) && umi_rad_id_valid(item->designer_type);}
