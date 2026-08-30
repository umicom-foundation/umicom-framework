/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/distraction_free.h
 *
 * PURPOSE:
 *   Resolve which editor chrome elements remain visible in distraction-free mode.
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
#ifndef UMICOM_EDITOR_WORKBENCH_DISTRACTION_FREE_H
#define UMICOM_EDITOR_WORKBENCH_DISTRACTION_FREE_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbDistractionFree { bool enabled; bool show_tabs; bool show_breadcrumbs; bool show_minimap; bool show_status; } UmiEditorWbDistractionFree;
void umi_editor_wb_distraction_free_resolve(UmiEditorWbDistractionFree *state,bool enabled);

#ifdef __cplusplus
}
#endif
#endif
