/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/minimap_model.c
 *
 * PURPOSE:
 *   Map document line positions onto a compact minimap coordinate space.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/minimap_model.h"

UmiStatus umi_editor_wb_minimap_model_init(UmiEditorWbMinimapModel *m,uint32_t lines,int32_t h){if(m==NULL||lines==0U||h<=0)return UMI_STATUS_INVALID_ARGUMENT;m->document_lines=lines;m->pixel_height=h;return UMI_STATUS_OK;}
int32_t umi_editor_wb_minimap_model_y_for_line(const UmiEditorWbMinimapModel *m,uint32_t line){if(m==NULL||m->document_lines==0U||line==0U)return -1;if(line>m->document_lines)line=m->document_lines;uint64_t numerator=(uint64_t)(line-1U)*(uint64_t)(uint32_t)m->pixel_height;return (int32_t)(numerator/(uint64_t)m->document_lines);}
