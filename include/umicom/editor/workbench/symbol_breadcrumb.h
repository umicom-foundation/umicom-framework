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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_SYMBOL_BREADCRUMB_H
#define UMICOM_EDITOR_WORKBENCH_SYMBOL_BREADCRUMB_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb symbol breadcrumb data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbSymbolBreadcrumb { char items[UMI_EDITOR_WB_MAX_SEGMENTS][UMI_EDITOR_WB_TEXT_CAPACITY]; uint32_t depth[UMI_EDITOR_WB_MAX_SEGMENTS]; size_t count; size_t active_index; } UmiEditorWbSymbolBreadcrumb;
/**
 * Initialise editor wb symbol breadcrumb from caller-provided values so later operations
 * receive a known state.
 */
void umi_editor_wb_symbol_breadcrumb_init(UmiEditorWbSymbolBreadcrumb *state);
/**
 * Add editor wb symbol breadcrumb only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_editor_wb_symbol_breadcrumb_append(UmiEditorWbSymbolBreadcrumb *state,const char *text,uint32_t depth);
/**
 * Provide the editor wb symbol breadcrumb activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_wb_symbol_breadcrumb_activate(UmiEditorWbSymbolBreadcrumb *state,size_t index);

#ifdef __cplusplus
}
#endif
#endif
