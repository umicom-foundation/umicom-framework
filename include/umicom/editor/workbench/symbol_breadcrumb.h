/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/symbol_breadcrumb.h
 *
 * PURPOSE:
 *   Represent a semantic symbol path distinct from the filesystem breadcrumb.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_SYMBOL_BREADCRUMB_H
#define UMICOM_EDITOR_WORKBENCH_SYMBOL_BREADCRUMB_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbSymbolBreadcrumb { char items[UMI_EDITOR_WB_MAX_SEGMENTS][UMI_EDITOR_WB_TEXT_CAPACITY]; uint32_t depth[UMI_EDITOR_WB_MAX_SEGMENTS]; size_t count; size_t active_index; } UmiEditorWbSymbolBreadcrumb;
void umi_editor_wb_symbol_breadcrumb_init(UmiEditorWbSymbolBreadcrumb *state);
UmiStatus umi_editor_wb_symbol_breadcrumb_append(UmiEditorWbSymbolBreadcrumb *state,const char *text,uint32_t depth);
UmiStatus umi_editor_wb_symbol_breadcrumb_activate(UmiEditorWbSymbolBreadcrumb *state,size_t index);

#ifdef __cplusplus
}
#endif
#endif
