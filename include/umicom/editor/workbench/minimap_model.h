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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_MINIMAP_MODEL_H
#define UMICOM_EDITOR_WORKBENCH_MINIMAP_MODEL_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbMinimapModel { uint32_t document_lines; int32_t pixel_height; } UmiEditorWbMinimapModel;
UmiStatus umi_editor_wb_minimap_model_init(UmiEditorWbMinimapModel *model,uint32_t document_lines,int32_t pixel_height);
int32_t umi_editor_wb_minimap_model_y_for_line(const UmiEditorWbMinimapModel *model,uint32_t line);

#ifdef __cplusplus
}
#endif
#endif
