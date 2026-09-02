/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/event_binding.c
 *
 * PURPOSE:
 *   Bind a semantic component event to a Framework command identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/event_binding.h"
#include <string.h>
/*
 * Initialise rad event binding from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_event_binding_init(UmiRadEventBinding *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->binding_id, sizeof item->binding_id, "event_binding");
    (void)umi_rad_copy_text(item->component_id, sizeof item->component_id, "event_binding");
    (void)umi_rad_copy_text(item->event_id, sizeof item->event_id, "event_binding");
    (void)umi_rad_copy_text(item->command_id, sizeof item->command_id, "event_binding");
    item->enabled = true;
    return UMI_STATUS_OK;
}
/* Check that rad event binding satisfies its contract before another service relies on it. */
int umi_rad_event_binding_is_valid(const UmiRadEventBinding *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->binding_id) && umi_rad_id_valid(item->event_id) && umi_rad_id_valid(item->command_id);}
