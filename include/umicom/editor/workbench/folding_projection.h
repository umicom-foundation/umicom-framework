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

/**
 * Represent the editor wb folding projection data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbFoldingProjection { UmiEditorWbRange folds[UMI_EDITOR_WB_MAX_SEGMENTS]; size_t count; } UmiEditorWbFoldingProjection;
/**
 * Initialise editor wb folding projection from caller-provided values so later operations
 * receive a known state.
 */
void umi_editor_wb_folding_projection_init(UmiEditorWbFoldingProjection *projection);
/**
 * Add editor wb folding projection only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_editor_wb_folding_projection_add(UmiEditorWbFoldingProjection *projection,uint32_t first_line,uint32_t last_line);
/**
 * Provide the editor wb folding projection line hidden operation used by this module and
 * its client applications.
 */
int umi_editor_wb_folding_projection_line_hidden(const UmiEditorWbFoldingProjection *projection,uint32_t line);

#ifdef __cplusplus
}
#endif
#endif
