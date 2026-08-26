/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_appearance.h
 *
 * PURPOSE:
 *   Describe editor-specific appearance choices on top of Framework appearance semantics.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_APPEARANCE_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_APPEARANCE_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbEditorAppearance { char id[UMI_EDITOR_WB_ID_CAPACITY]; char text[UMI_EDITOR_WB_TEXT_CAPACITY]; uint64_t primary; uint64_t secondary; bool enabled; } UmiEditorWbEditorAppearance;
UmiStatus umi_editor_wb_editor_appearance_init(UmiEditorWbEditorAppearance *state,const char *id,const char *text); UmiStatus umi_editor_wb_editor_appearance_set_values(UmiEditorWbEditorAppearance *state,uint64_t primary,uint64_t secondary,bool enabled); int umi_editor_wb_editor_appearance_valid(const UmiEditorWbEditorAppearance *state);

#ifdef __cplusplus
}
#endif
#endif
