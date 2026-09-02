/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/action_binding.c
 *
 * PURPOSE:
 *   Bind a designer action surface to a Framework command and target.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/action_binding.h"
#include <string.h>
/*
 * Initialise rad action binding from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_action_binding_init(UmiRadActionBinding *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->action_id, sizeof item->action_id, "action_binding");
    (void)umi_rad_copy_text(item->command_id, sizeof item->command_id, "action_binding");
    (void)umi_rad_copy_text(item->target_id, sizeof item->target_id, "action_binding");
    item->enabled = true;
    return UMI_STATUS_OK;
}
/*
 * Check that rad action binding satisfies its contract before another service relies on
 * it.
 */
int umi_rad_action_binding_is_valid(const UmiRadActionBinding *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->action_id) && umi_rad_id_valid(item->command_id);}
