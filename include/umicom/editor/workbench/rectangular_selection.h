/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/rectangular_selection.h
 *
 * PURPOSE:
 *   Normalize block/column selection coordinates.
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
#ifndef UMICOM_EDITOR_WORKBENCH_RECTANGULAR_SELECTION_H
#define UMICOM_EDITOR_WORKBENCH_RECTANGULAR_SELECTION_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbRectangularSelection { uint32_t first_line; uint32_t last_line; uint32_t first_column; uint32_t last_column; } UmiEditorWbRectangularSelection;
UmiStatus umi_editor_wb_rectangular_selection_init(UmiEditorWbRectangularSelection *selection,uint32_t line_a,uint32_t column_a,uint32_t line_b,uint32_t column_b);
size_t umi_editor_wb_rectangular_selection_line_count(const UmiEditorWbRectangularSelection *selection);

#ifdef __cplusplus
}
#endif
#endif
