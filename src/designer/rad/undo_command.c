/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/undo_command.c
 *
 * PURPOSE:
 *   Represent a reversible designer mutation without toolkit dependencies.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/undo_command.h"
#include <string.h>
UmiStatus umi_rad_undo_command_init(UmiRadUndoCommand *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->command_id, sizeof item->command_id, "undo_command");
    (void)umi_rad_copy_text(item->target_id, sizeof item->target_id, "undo_command");
    (void)umi_rad_copy_text(item->before_value, sizeof item->before_value, "undo_command");
    (void)umi_rad_copy_text(item->after_value, sizeof item->after_value, "undo_command");
    return UMI_STATUS_OK;
}
int umi_rad_undo_command_is_valid(const UmiRadUndoCommand *item){if(item==NULL)return 0;return umi_rad_id_valid(item->command_id) && umi_rad_id_valid(item->target_id);}
