/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_split.h
 *
 * PURPOSE:
 *   Describe one horizontal or vertical editor split with a bounded ratio.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_SPLIT_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_SPLIT_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbEditorSplit { char split_id[UMI_EDITOR_WB_ID_CAPACITY]; UmiEditorWbOrientation orientation; double ratio; } UmiEditorWbEditorSplit;
UmiStatus umi_editor_wb_editor_split_init(UmiEditorWbEditorSplit *split,const char *id,UmiEditorWbOrientation orientation,double ratio);
UmiStatus umi_editor_wb_editor_split_set_ratio(UmiEditorWbEditorSplit *split,double ratio);
int umi_editor_wb_editor_split_valid(const UmiEditorWbEditorSplit *split);

#ifdef __cplusplus
}
#endif
#endif
