/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/overview_ruler.h
 *
 * PURPOSE:
 *   Maintain diagnostics/bookmark/search markers for an editor overview ruler.
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
#ifndef UMICOM_EDITOR_WORKBENCH_OVERVIEW_RULER_H
#define UMICOM_EDITOR_WORKBENCH_OVERVIEW_RULER_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb overview marker data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbOverviewMarker { uint32_t line; UmiEditorWbSeverity severity; } UmiEditorWbOverviewMarker; typedef struct UmiEditorWbOverviewRuler { UmiEditorWbOverviewMarker markers[UMI_EDITOR_WB_MAX_SEGMENTS]; size_t count; } UmiEditorWbOverviewRuler;
/**
 * Initialise editor wb overview ruler from caller-provided values so later operations
 * receive a known state.
 */
void umi_editor_wb_overview_ruler_init(UmiEditorWbOverviewRuler *state); UmiStatus umi_editor_wb_overview_ruler_add(UmiEditorWbOverviewRuler *state,uint32_t line,UmiEditorWbSeverity severity);

#ifdef __cplusplus
}
#endif
#endif
