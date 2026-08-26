/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/recently_closed_editor.h
 *
 * PURPOSE:
 *   Maintain a bounded LIFO history of recently closed resources.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_RECENTLY_CLOSED_EDITOR_H
#define UMICOM_EDITOR_WORKBENCH_RECENTLY_CLOSED_EDITOR_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbRecentlyClosedEditor { char paths[UMI_EDITOR_WB_MAX_ITEMS][UMI_EDITOR_WB_PATH_CAPACITY]; size_t count; } UmiEditorWbRecentlyClosedEditor;
void umi_editor_wb_recently_closed_editor_init(UmiEditorWbRecentlyClosedEditor *history);
UmiStatus umi_editor_wb_recently_closed_editor_push(UmiEditorWbRecentlyClosedEditor *history,const char *path);
UmiStatus umi_editor_wb_recently_closed_editor_pop(UmiEditorWbRecentlyClosedEditor *history,char *out_path,size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
