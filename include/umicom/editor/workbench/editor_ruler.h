/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_ruler.h
 *
 * PURPOSE:
 *   Maintain reusable preferred-column ruler positions.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_RULER_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_RULER_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbEditorRuler { char items[UMI_EDITOR_WB_MAX_SEGMENTS][UMI_EDITOR_WB_TEXT_CAPACITY]; uint32_t depth[UMI_EDITOR_WB_MAX_SEGMENTS]; size_t count; size_t active_index; } UmiEditorWbEditorRuler;
void umi_editor_wb_editor_ruler_init(UmiEditorWbEditorRuler *state);
UmiStatus umi_editor_wb_editor_ruler_append(UmiEditorWbEditorRuler *state,const char *text,uint32_t depth);
UmiStatus umi_editor_wb_editor_ruler_activate(UmiEditorWbEditorRuler *state,size_t index);

#ifdef __cplusplus
}
#endif
#endif
