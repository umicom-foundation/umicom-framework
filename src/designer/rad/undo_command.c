/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/undo_command.c
 *
 * PURPOSE:
 *   Represent a reversible designer mutation without toolkit dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/undo_command.h"
#include <string.h>
/*
 * Initialise rad undo command from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_undo_command_init(UmiRadUndoCommand *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->command_id, sizeof item->command_id, "undo_command");
    (void)umi_rad_copy_text(item->target_id, sizeof item->target_id, "undo_command");
    (void)umi_rad_copy_text(item->before_value, sizeof item->before_value, "undo_command");
    (void)umi_rad_copy_text(item->after_value, sizeof item->after_value, "undo_command");
    return UMI_STATUS_OK;
}
/* Check that rad undo command satisfies its contract before another service relies on it. */
int umi_rad_undo_command_is_valid(const UmiRadUndoCommand *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->command_id) && umi_rad_id_valid(item->target_id);}
