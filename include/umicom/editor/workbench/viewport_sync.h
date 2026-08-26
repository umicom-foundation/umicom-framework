/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/viewport_sync.h
 *
 * PURPOSE:
 *   Synchronize selected viewport dimensions between linked editors.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_VIEWPORT_SYNC_H
#define UMICOM_EDITOR_WORKBENCH_VIEWPORT_SYNC_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbViewportSync { bool horizontal; bool vertical; double master_horizontal; double master_vertical; double follower_horizontal; double follower_vertical; } UmiEditorWbViewportSync;
void umi_editor_wb_viewport_sync_apply(UmiEditorWbViewportSync *state,double horizontal,double vertical);

#ifdef __cplusplus
}
#endif
#endif
