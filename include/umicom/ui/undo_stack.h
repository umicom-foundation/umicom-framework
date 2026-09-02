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
/**
 * Represent the ui undo entry data shared with callers of this public contract.
 */
typedef struct UmiUiUndoEntry { char id[128]; char label[256]; char forward_payload[1024]; char reverse_payload[1024]; uint64_t sequence; } UmiUiUndoEntry;
/**
 * Represent the ui undo stack data shared with callers of this public contract.
 */
typedef struct UmiUiUndoStack UmiUiUndoStack;
/**
 * Initialise ui undo stack from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ui_undo_stack_create(UmiUiUndoStack **out_stack);
/**
 * Release or reset state held by ui undo stack so the same storage can be reused safely.
 */
void umi_ui_undo_stack_destroy(UmiUiUndoStack *stack);
/**
 * Provide the ui undo stack push operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_undo_stack_push(UmiUiUndoStack *stack,const UmiUiUndoEntry *entry);
/**
 * Provide the ui undo stack undo operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_undo_stack_undo(UmiUiUndoStack *stack,UmiUiUndoEntry *out_entry);
/**
 * Provide the ui undo stack redo operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_undo_stack_redo(UmiUiUndoStack *stack,UmiUiUndoEntry *out_entry);
/**
 * Release or reset state held by ui undo stack so the same storage can be reused safely.
 */
void umi_ui_undo_stack_clear(UmiUiUndoStack *stack);
/**
 * Return the number of records represented by ui undo stack undo without changing their
 * state.
 */
size_t umi_ui_undo_stack_undo_count(const UmiUiUndoStack *stack);
/**
 * Return the number of records represented by ui undo stack redo without changing their
 * state.
 */
size_t umi_ui_undo_stack_redo_count(const UmiUiUndoStack *stack);
/**
 * Provide the ui undo stack revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_undo_stack_revision(const UmiUiUndoStack *stack);
#ifdef __cplusplus
}
#endif
#endif
