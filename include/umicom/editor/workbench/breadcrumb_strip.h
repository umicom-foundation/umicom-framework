/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/breadcrumb_strip.h
 *
 * PURPOSE:
 *   Compose ordered file/symbol breadcrumb segments for an editor surface.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_BREADCRUMB_STRIP_H
#define UMICOM_EDITOR_WORKBENCH_BREADCRUMB_STRIP_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbBreadcrumbStrip { char items[UMI_EDITOR_WB_MAX_SEGMENTS][UMI_EDITOR_WB_TEXT_CAPACITY]; uint32_t depth[UMI_EDITOR_WB_MAX_SEGMENTS]; size_t count; size_t active_index; } UmiEditorWbBreadcrumbStrip;
void umi_editor_wb_breadcrumb_strip_init(UmiEditorWbBreadcrumbStrip *state);
UmiStatus umi_editor_wb_breadcrumb_strip_append(UmiEditorWbBreadcrumbStrip *state,const char *text,uint32_t depth);
UmiStatus umi_editor_wb_breadcrumb_strip_activate(UmiEditorWbBreadcrumbStrip *state,size_t index);

#ifdef __cplusplus
}
#endif
#endif
