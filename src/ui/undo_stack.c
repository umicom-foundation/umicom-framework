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
/*
 * Initialise ui undo stack from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ui_undo_stack_create(UmiUiUndoStack **out){UmiUiUndoStack*s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!out)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;s=calloc(1U,sizeof(*s));/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s)return UMI_STATUS_OUT_OF_MEMORY;s->revision=1U;s->next_sequence=1U;*out=s;return UMI_STATUS_OK;}
/* Release or reset state held by ui undo stack so the same storage can be reused safely. */
void umi_ui_undo_stack_destroy(UmiUiUndoStack*s){free(s);}
/*
 * Provide the ui undo stack push operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_undo_stack_push(UmiUiUndoStack*s,const UmiUiUndoEntry*e){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!s||!e||e->id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s->cursor<s->count)s->count=s->cursor;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s->count>=UMI_UI_UNDO_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;s->items[s->count]=*e;s->items[s->count].sequence=s->next_sequence++;s->count++;s->cursor=s->count;s->revision++;return UMI_STATUS_OK;}
/*
 * Provide the ui undo stack undo operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_undo_stack_undo(UmiUiUndoStack*s,UmiUiUndoEntry*out){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s||!out)return UMI_STATUS_INVALID_ARGUMENT;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s->cursor==0U)return UMI_STATUS_NOT_FOUND;s->cursor--;*out=s->items[s->cursor];s->revision++;return UMI_STATUS_OK;}
/*
 * Provide the ui undo stack redo operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_undo_stack_redo(UmiUiUndoStack*s,UmiUiUndoEntry*out){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!s||!out)return UMI_STATUS_INVALID_ARGUMENT;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s->cursor>=s->count)return UMI_STATUS_NOT_FOUND;*out=s->items[s->cursor++];s->revision++;return UMI_STATUS_OK;}
/* Release or reset state held by ui undo stack so the same storage can be reused safely. */
void umi_ui_undo_stack_clear(UmiUiUndoStack*s){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s){s->count=0U;s->cursor=0U;s->revision++;}}
/*
 * Return the number of records represented by ui undo stack undo without changing their
 * state.
 */
size_t umi_ui_undo_stack_undo_count(const UmiUiUndoStack*s){return s?s->cursor:0U;}
/*
 * Return the number of records represented by ui undo stack redo without changing their
 * state.
 */
size_t umi_ui_undo_stack_redo_count(const UmiUiUndoStack*s){return s?s->count-s->cursor:0U;}
/*
 * Provide the ui undo stack revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_undo_stack_revision(const UmiUiUndoStack*s){return s?s->revision:0U;}
