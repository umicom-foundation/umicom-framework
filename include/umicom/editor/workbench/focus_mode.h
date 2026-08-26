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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_FOCUS_MODE_H
#define UMICOM_EDITOR_WORKBENCH_FOCUS_MODE_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbFocusMode { char item_id[UMI_EDITOR_WB_ID_CAPACITY]; bool enabled; bool promoted; uint64_t revision; } UmiEditorWbFocusMode;
UmiStatus umi_editor_wb_focus_mode_init(UmiEditorWbFocusMode *state,const char *item_id,bool enabled);
UmiStatus umi_editor_wb_focus_mode_set(UmiEditorWbFocusMode *state,bool enabled);
int umi_editor_wb_focus_mode_valid(const UmiEditorWbFocusMode *state);

#ifdef __cplusplus
}
#endif
#endif
