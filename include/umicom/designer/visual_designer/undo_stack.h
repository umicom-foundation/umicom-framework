/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/undo_stack.h
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
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_UNDO_STACK_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_UNDO_STACK_H
#include "umicom/designer/visual_designer/undo_command.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer undo stack data shared with callers of this public contract.
 */
typedef struct UmiRadUndoStack { UmiRadUndoCommand commands[UMI_RAD_MAX_HISTORY]; size_t count; size_t cursor; } UmiRadUndoStack;
/**
 * Initialise visual designer undo stack from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_undo_stack_init(UmiRadUndoStack *stack);
/**
 * Provide the visual designer undo stack push operation used by this module and its client
 * applications.
 */
UmiStatus umi_rad_undo_stack_push(UmiRadUndoStack *stack,const UmiRadUndoCommand *command);
/**
 * Provide the visual designer undo stack undo operation used by this module and its client
 * applications.
 */
const UmiRadUndoCommand *umi_rad_undo_stack_undo(UmiRadUndoStack *stack);
/**
 * Provide the visual designer undo stack redo operation used by this module and its client
 * applications.
 */
const UmiRadUndoCommand *umi_rad_undo_stack_redo(UmiRadUndoStack *stack);
#ifdef __cplusplus
}
#endif
#endif
