/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/bookmark_panel.h
 *
 * PURPOSE:
 *   Maintain a reusable bookmark-panel model over stable source locations.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_BOOKMARK_PANEL_H
#define UMICOM_EDITOR_WORKBENCH_BOOKMARK_PANEL_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbBookmarkPanel { char ids[UMI_EDITOR_WB_MAX_ITEMS][UMI_EDITOR_WB_ID_CAPACITY]; size_t count; uint64_t revision; } UmiEditorWbBookmarkPanel;
void umi_editor_wb_bookmark_panel_init(UmiEditorWbBookmarkPanel *state);
UmiStatus umi_editor_wb_bookmark_panel_add(UmiEditorWbBookmarkPanel *state,const char *id);
int umi_editor_wb_bookmark_panel_contains(const UmiEditorWbBookmarkPanel *state,const char *id);
UmiStatus umi_editor_wb_bookmark_panel_remove(UmiEditorWbBookmarkPanel *state,const char *id);

#ifdef __cplusplus
}
#endif
#endif
