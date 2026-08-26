/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/synchronized_editors.h
 *
 * PURPOSE:
 *   Maintain a bounded set of editor surfaces participating in synchronization.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_SYNCHRONIZED_EDITORS_H
#define UMICOM_EDITOR_WORKBENCH_SYNCHRONIZED_EDITORS_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbSynchronizedEditors { char items[UMI_EDITOR_WB_MAX_SEGMENTS][UMI_EDITOR_WB_TEXT_CAPACITY]; uint32_t depth[UMI_EDITOR_WB_MAX_SEGMENTS]; size_t count; size_t active_index; } UmiEditorWbSynchronizedEditors;
void umi_editor_wb_synchronized_editors_init(UmiEditorWbSynchronizedEditors *state);
UmiStatus umi_editor_wb_synchronized_editors_append(UmiEditorWbSynchronizedEditors *state,const char *text,uint32_t depth);
UmiStatus umi_editor_wb_synchronized_editors_activate(UmiEditorWbSynchronizedEditors *state,size_t index);

#ifdef __cplusplus
}
#endif
#endif
