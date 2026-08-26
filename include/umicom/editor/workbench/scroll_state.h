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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_SCROLL_STATE_H
#define UMICOM_EDITOR_WORKBENCH_SCROLL_STATE_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbScrollState { double horizontal; double vertical; double max_horizontal; double max_vertical; } UmiEditorWbScrollState;
UmiStatus umi_editor_wb_scroll_state_init(UmiEditorWbScrollState *state,double max_horizontal,double max_vertical);
UmiStatus umi_editor_wb_scroll_state_set(UmiEditorWbScrollState *state,double horizontal,double vertical);

#ifdef __cplusplus
}
#endif
#endif
