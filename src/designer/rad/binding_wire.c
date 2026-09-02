/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/binding_wire.c
 *
 * PURPOSE:
 *   Represent a directed visual binding wire between endpoints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/binding_wire.h"
#include <string.h>
/*
 * Initialise rad binding wire from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_binding_wire_init(UmiRadBindingWire *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->wire_id, sizeof item->wire_id, "binding_wire");
    (void)umi_rad_copy_text(item->source_node_id, sizeof item->source_node_id, "binding_wire");
    (void)umi_rad_copy_text(item->target_node_id, sizeof item->target_node_id, "binding_wire");
    item->enabled = true;
    return UMI_STATUS_OK;
}
/* Check that rad binding wire satisfies its contract before another service relies on it. */
int umi_rad_binding_wire_is_valid(const UmiRadBindingWire *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->wire_id) && umi_rad_id_valid(item->source_node_id) && umi_rad_id_valid(item->target_node_id);}
