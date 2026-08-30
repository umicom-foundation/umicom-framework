/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/undo_command.h
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
#ifndef UMICOM_DESIGNER_RAD_UNDO_COMMAND_H
#define UMICOM_DESIGNER_RAD_UNDO_COMMAND_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadUndoCommand {
    char command_id[UMI_RAD_ID_CAPACITY];
    char target_id[UMI_RAD_ID_CAPACITY];
    char before_value[UMI_RAD_VALUE_CAPACITY];
    char after_value[UMI_RAD_VALUE_CAPACITY];
} UmiRadUndoCommand;
UmiStatus umi_rad_undo_command_init(UmiRadUndoCommand *item);
int umi_rad_undo_command_is_valid(const UmiRadUndoCommand *item);
#ifdef __cplusplus
}
#endif
#endif
