/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/active_editor.h
 *
 * PURPOSE:
 *   Track the active editor item and group as one coherent state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_ACTIVE_EDITOR_H
#define UMICOM_EDITOR_WORKBENCH_ACTIVE_EDITOR_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbActiveEditor { char item_id[UMI_EDITOR_WB_ID_CAPACITY]; char group_id[UMI_EDITOR_WB_ID_CAPACITY]; uint64_t revision; } UmiEditorWbActiveEditor;
UmiStatus umi_editor_wb_active_editor_set(UmiEditorWbActiveEditor *state,const char *item_id,const char *group_id); void umi_editor_wb_active_editor_clear(UmiEditorWbActiveEditor *state);

#ifdef __cplusplus
}
#endif
#endif
