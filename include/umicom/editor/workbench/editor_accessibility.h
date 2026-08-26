/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_accessibility.h
 *
 * PURPOSE:
 *   Describe screen-reader and keyboard-access metadata for an editor surface.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_ACCESSIBILITY_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_ACCESSIBILITY_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbEditorAccessibility { char id[UMI_EDITOR_WB_ID_CAPACITY]; char text[UMI_EDITOR_WB_TEXT_CAPACITY]; uint64_t primary; uint64_t secondary; bool enabled; } UmiEditorWbEditorAccessibility;
UmiStatus umi_editor_wb_editor_accessibility_init(UmiEditorWbEditorAccessibility *state,const char *id,const char *text); UmiStatus umi_editor_wb_editor_accessibility_set_values(UmiEditorWbEditorAccessibility *state,uint64_t primary,uint64_t secondary,bool enabled); int umi_editor_wb_editor_accessibility_valid(const UmiEditorWbEditorAccessibility *state);

#ifdef __cplusplus
}
#endif
#endif
