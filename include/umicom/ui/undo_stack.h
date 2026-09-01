/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/undo_stack.h
 *
 * PURPOSE:
 *   Define a bounded undo/redo journal for reusable editing and designer workflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract is toolkit-neutral and intended for reuse by Studio,
 * Trader, TMS and future Umicom applications without exposing GUI objects.
 */
#ifndef FRAMEWORK_INCLUDE_UMICOM_UI_UNDO_STACK_H
#define FRAMEWORK_INCLUDE_UMICOM_UI_UNDO_STACK_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_UI_UNDO_CAPACITY 512U
typedef struct UmiUiUndoEntry { char id[128]; char label[256]; char forward_payload[1024]; char reverse_payload[1024]; uint64_t sequence; } UmiUiUndoEntry;
typedef struct UmiUiUndoStack UmiUiUndoStack;
UmiStatus umi_ui_undo_stack_create(UmiUiUndoStack **out_stack);
void umi_ui_undo_stack_destroy(UmiUiUndoStack *stack);
UmiStatus umi_ui_undo_stack_push(UmiUiUndoStack *stack,const UmiUiUndoEntry *entry);
UmiStatus umi_ui_undo_stack_undo(UmiUiUndoStack *stack,UmiUiUndoEntry *out_entry);
UmiStatus umi_ui_undo_stack_redo(UmiUiUndoStack *stack,UmiUiUndoEntry *out_entry);
void umi_ui_undo_stack_clear(UmiUiUndoStack *stack);
size_t umi_ui_undo_stack_undo_count(const UmiUiUndoStack *stack);
size_t umi_ui_undo_stack_redo_count(const UmiUiUndoStack *stack);
uint64_t umi_ui_undo_stack_revision(const UmiUiUndoStack *stack);
#ifdef __cplusplus
}
#endif
#endif
