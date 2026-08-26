/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/component_instance.c
 *
 * PURPOSE:
 *   Represent one semantic component instance on a designer document.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/component_instance.h"
#include <string.h>
UmiStatus umi_rad_component_instance_init(UmiRadComponentInstance *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->component_id, sizeof item->component_id, "component_instance");
    (void)umi_rad_copy_text(item->component_type, sizeof item->component_type, "component_instance");
    (void)umi_rad_copy_text(item->parent_id, sizeof item->parent_id, "component_instance");
    item->visible = true;
    return UMI_STATUS_OK;
}
int umi_rad_component_instance_is_valid(const UmiRadComponentInstance *item){if(item==NULL)return 0;return umi_rad_id_valid(item->component_id) && umi_rad_id_valid(item->component_type);}
