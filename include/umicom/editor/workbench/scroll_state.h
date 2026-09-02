/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/scroll_state.h
 *
 * PURPOSE:
 *   Track and clamp horizontal/vertical editor scroll offsets.
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
#ifndef UMICOM_EDITOR_WORKBENCH_SCROLL_STATE_H
#define UMICOM_EDITOR_WORKBENCH_SCROLL_STATE_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb scroll state data shared with callers of this public contract.
 */
typedef struct UmiEditorWbScrollState { double horizontal; double vertical; double max_horizontal; double max_vertical; } UmiEditorWbScrollState;
/**
 * Initialise editor wb scroll state from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_scroll_state_init(UmiEditorWbScrollState *state,double max_horizontal,double max_vertical);
/**
 * Copy editor wb scroll state into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_editor_wb_scroll_state_set(UmiEditorWbScrollState *state,double horizontal,double vertical);

#ifdef __cplusplus
}
#endif
#endif
