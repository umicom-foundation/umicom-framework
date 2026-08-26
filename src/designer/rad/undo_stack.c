/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/undo_stack.c
 *
 * PURPOSE:
 *   Maintain deterministic undo/redo history for visual authoring operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/undo_stack.h"
#include <string.h>
UmiStatus umi_rad_undo_stack_init(UmiRadUndoStack *s){if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);return UMI_STATUS_OK;}
UmiStatus umi_rad_undo_stack_push(UmiRadUndoStack *s,const UmiRadUndoCommand *c){if(s==NULL||c==NULL||!umi_rad_undo_command_is_valid(c))return UMI_STATUS_INVALID_ARGUMENT;if(s->cursor< s->count)s->count=s->cursor;if(s->count>=UMI_RAD_MAX_HISTORY)return UMI_STATUS_CAPACITY_EXCEEDED;s->commands[s->count++]=*c;s->cursor=s->count;return UMI_STATUS_OK;}
const UmiRadUndoCommand *umi_rad_undo_stack_undo(UmiRadUndoStack *s){if(s==NULL||s->cursor==0U)return NULL;s->cursor--;return &s->commands[s->cursor];}
const UmiRadUndoCommand *umi_rad_undo_stack_redo(UmiRadUndoStack *s){const UmiRadUndoCommand *c;if(s==NULL||s->cursor>=s->count)return NULL;c=&s->commands[s->cursor];s->cursor++;return c;}
