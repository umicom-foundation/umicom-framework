/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/property_binding.c
 *
 * PURPOSE:
 *   Describe a visual property binding backed by the canonical reactive UI state layer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/property_binding.h"
#include <string.h>
/*
 * Initialise rad property binding from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_property_binding_init(UmiRadPropertyBinding *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->binding_id, sizeof item->binding_id, "property_binding");
    (void)umi_rad_copy_text(item->source_path, sizeof item->source_path, "property_binding");
    (void)umi_rad_copy_text(item->target_path, sizeof item->target_path, "property_binding");
    item->enabled = true;
    return UMI_STATUS_OK;
}
/*
 * Check that rad property binding satisfies its contract before another service relies on
 * it.
 */
int umi_rad_property_binding_is_valid(const UmiRadPropertyBinding *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->binding_id) && item->source_path[0] != '\0' && item->target_path[0] != '\0';}
