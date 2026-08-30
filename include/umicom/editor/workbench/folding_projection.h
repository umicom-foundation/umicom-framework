/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/folding_projection.h
 *
 * PURPOSE:
 *   Project fold ranges into line-visibility decisions without owning language parsing.
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
#ifndef UMICOM_EDITOR_WORKBENCH_FOLDING_PROJECTION_H
#define UMICOM_EDITOR_WORKBENCH_FOLDING_PROJECTION_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbFoldingProjection { UmiEditorWbRange folds[UMI_EDITOR_WB_MAX_SEGMENTS]; size_t count; } UmiEditorWbFoldingProjection;
void umi_editor_wb_folding_projection_init(UmiEditorWbFoldingProjection *projection);
UmiStatus umi_editor_wb_folding_projection_add(UmiEditorWbFoldingProjection *projection,uint32_t first_line,uint32_t last_line);
int umi_editor_wb_folding_projection_line_hidden(const UmiEditorWbFoldingProjection *projection,uint32_t line);

#ifdef __cplusplus
}
#endif
#endif
