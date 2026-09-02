/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/undo_stack.c
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
#include "umicom/designer/visual_designer/undo_stack.h"
#include <string.h>
/*
 * Initialise visual designer undo stack from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_undo_stack_init(UmiRadUndoStack *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);return UMI_STATUS_OK;}
/*
 * Provide the visual designer undo stack push operation used by this module and its client
 * applications.
 */
UmiStatus umi_rad_undo_stack_push(UmiRadUndoStack *s,const UmiRadUndoCommand *c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||c==NULL||!umi_rad_undo_command_is_valid(c))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->cursor< s->count)s->count=s->cursor;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=UMI_RAD_MAX_HISTORY)return UMI_STATUS_CAPACITY_EXCEEDED;s->commands[s->count++]=*c;s->cursor=s->count;return UMI_STATUS_OK;}
/*
 * Provide the visual designer undo stack undo operation used by this module and its client
 * applications.
 */
const UmiRadUndoCommand *umi_rad_undo_stack_undo(UmiRadUndoStack *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||s->cursor==0U)return NULL;s->cursor--;return &s->commands[s->cursor];}
/*
 * Provide the visual designer undo stack redo operation used by this module and its client
 * applications.
 */
const UmiRadUndoCommand *umi_rad_undo_stack_redo(UmiRadUndoStack *s){const UmiRadUndoCommand *c;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||s->cursor>=s->count)return NULL;c=&s->commands[s->cursor];s->cursor++;return c;}
