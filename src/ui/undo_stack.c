/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/undo_stack.c
 *
 * PURPOSE:
 *   Implement a bounded undo/redo journal for reusable editing and designer workflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This implementation owns only its documented child models. Product-specific
 * business data and toolkit objects remain outside this reusable Framework layer.
 */
#include "umicom/ui/undo_stack.h"
#include <stdlib.h>
struct UmiUiUndoStack { UmiUiUndoEntry items[UMI_UI_UNDO_CAPACITY]; size_t count; size_t cursor; uint64_t revision; uint64_t next_sequence; };
UmiStatus umi_ui_undo_stack_create(UmiUiUndoStack **out){UmiUiUndoStack*s;if(!out)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;s=calloc(1U,sizeof(*s));if(!s)return UMI_STATUS_OUT_OF_MEMORY;s->revision=1U;s->next_sequence=1U;*out=s;return UMI_STATUS_OK;}
void umi_ui_undo_stack_destroy(UmiUiUndoStack*s){free(s);}
UmiStatus umi_ui_undo_stack_push(UmiUiUndoStack*s,const UmiUiUndoEntry*e){if(!s||!e||e->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;if(s->cursor<s->count)s->count=s->cursor;if(s->count>=UMI_UI_UNDO_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;s->items[s->count]=*e;s->items[s->count].sequence=s->next_sequence++;s->count++;s->cursor=s->count;s->revision++;return UMI_STATUS_OK;}
UmiStatus umi_ui_undo_stack_undo(UmiUiUndoStack*s,UmiUiUndoEntry*out){if(!s||!out)return UMI_STATUS_INVALID_ARGUMENT;if(s->cursor==0U)return UMI_STATUS_NOT_FOUND;s->cursor--;*out=s->items[s->cursor];s->revision++;return UMI_STATUS_OK;}
UmiStatus umi_ui_undo_stack_redo(UmiUiUndoStack*s,UmiUiUndoEntry*out){if(!s||!out)return UMI_STATUS_INVALID_ARGUMENT;if(s->cursor>=s->count)return UMI_STATUS_NOT_FOUND;*out=s->items[s->cursor++];s->revision++;return UMI_STATUS_OK;}
void umi_ui_undo_stack_clear(UmiUiUndoStack*s){if(s){s->count=0U;s->cursor=0U;s->revision++;}}
size_t umi_ui_undo_stack_undo_count(const UmiUiUndoStack*s){return s?s->cursor:0U;}
size_t umi_ui_undo_stack_redo_count(const UmiUiUndoStack*s){return s?s->count-s->cursor:0U;}
uint64_t umi_ui_undo_stack_revision(const UmiUiUndoStack*s){return s?s->revision:0U;}
