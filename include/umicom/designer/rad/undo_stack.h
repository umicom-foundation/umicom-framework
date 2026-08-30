/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/undo_stack.h
 *
 * PURPOSE:
 *   Maintain deterministic undo/redo history for visual authoring operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_UNDO_STACK_H
#define UMICOM_DESIGNER_RAD_UNDO_STACK_H
#include "umicom/designer/rad/undo_command.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadUndoStack { UmiRadUndoCommand commands[UMI_RAD_MAX_HISTORY]; size_t count; size_t cursor; } UmiRadUndoStack;
UmiStatus umi_rad_undo_stack_init(UmiRadUndoStack *stack);
UmiStatus umi_rad_undo_stack_push(UmiRadUndoStack *stack,const UmiRadUndoCommand *command);
const UmiRadUndoCommand *umi_rad_undo_stack_undo(UmiRadUndoStack *stack);
const UmiRadUndoCommand *umi_rad_undo_stack_redo(UmiRadUndoStack *stack);
#ifdef __cplusplus
}
#endif
#endif
