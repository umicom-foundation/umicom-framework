/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/focus_mode.h
 *
 * PURPOSE:
 *   Track focus mode while preserving the previously visible workbench regions.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_FOCUS_MODE_H
#define UMICOM_EDITOR_WORKBENCH_FOCUS_MODE_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb focus mode data shared with callers of this public contract.
 */
typedef struct UmiEditorWbFocusMode { char item_id[UMI_EDITOR_WB_ID_CAPACITY]; bool enabled; bool promoted; uint64_t revision; } UmiEditorWbFocusMode;
/**
 * Initialise editor wb focus mode from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_editor_wb_focus_mode_init(UmiEditorWbFocusMode *state,const char *item_id,bool enabled);
/**
 * Copy editor wb focus mode into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_editor_wb_focus_mode_set(UmiEditorWbFocusMode *state,bool enabled);
/**
 * Check that editor wb focus mode satisfies its contract before another service relies on
 * it.
 */
int umi_editor_wb_focus_mode_valid(const UmiEditorWbFocusMode *state);

#ifdef __cplusplus
}
#endif
#endif
