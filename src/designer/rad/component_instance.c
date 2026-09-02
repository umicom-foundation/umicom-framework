/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/component_instance.c
 *
 * PURPOSE:
 *   Represent one semantic component instance on a designer document.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/component_instance.h"
#include <string.h>
/*
 * Initialise rad component instance from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_component_instance_init(UmiRadComponentInstance *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->component_id, sizeof item->component_id, "component_instance");
    (void)umi_rad_copy_text(item->component_type, sizeof item->component_type, "component_instance");
    (void)umi_rad_copy_text(item->parent_id, sizeof item->parent_id, "component_instance");
    item->visible = true;
    return UMI_STATUS_OK;
}
/*
 * Check that rad component instance satisfies its contract before another service relies
 * on it.
 */
int umi_rad_component_instance_is_valid(const UmiRadComponentInstance *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->component_id) && umi_rad_id_valid(item->component_type);}
