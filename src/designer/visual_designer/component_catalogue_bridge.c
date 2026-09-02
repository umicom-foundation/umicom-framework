/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/component_catalogue_bridge.c
 *
 * PURPOSE:
 *   Map Design System component identifiers to canonical designer component types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/component_catalogue_bridge.h"
#include <string.h>
/*
 * Initialise visual designer component catalogue bridge from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_rad_component_catalogue_bridge_init(UmiRadComponentCatalogueBridge *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->design_component_id, sizeof item->design_component_id, "component_catalogue_bridge");
    (void)umi_rad_copy_text(item->designer_type, sizeof item->designer_type, "component_catalogue_bridge");
    (void)umi_rad_copy_text(item->family, sizeof item->family, "component_catalogue_bridge");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer component catalogue bridge satisfies its contract before another service
 * relies on it.
 */
int umi_rad_component_catalogue_bridge_is_valid(const UmiRadComponentCatalogueBridge *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->design_component_id) && umi_rad_id_valid(item->designer_type);}
