/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/minimap_model.h
 *
 * PURPOSE:
 *   Map document line positions onto a compact minimap coordinate space.
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
#ifndef UMICOM_EDITOR_WORKBENCH_MINIMAP_MODEL_H
#define UMICOM_EDITOR_WORKBENCH_MINIMAP_MODEL_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb minimap model data shared with callers of this public contract.
 */
typedef struct UmiEditorWbMinimapModel { uint32_t document_lines; int32_t pixel_height; } UmiEditorWbMinimapModel;
/**
 * Initialise editor wb minimap model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_minimap_model_init(UmiEditorWbMinimapModel *model,uint32_t document_lines,int32_t pixel_height);
/**
 * Provide the editor wb minimap model y for line operation used by this module and its
 * client applications.
 */
int32_t umi_editor_wb_minimap_model_y_for_line(const UmiEditorWbMinimapModel *model,uint32_t line);

#ifdef __cplusplus
}
#endif
#endif
