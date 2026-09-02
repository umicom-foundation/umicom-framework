/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/minimap_model.c
 *
 * PURPOSE:
 *   Map document line positions onto a compact minimap coordinate space.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/minimap_model.h"

/*
 * Initialise editor wb minimap model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_minimap_model_init(UmiEditorWbMinimapModel *m,uint32_t lines,int32_t h){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||lines==0U||h<=0)return UMI_STATUS_INVALID_ARGUMENT;m->document_lines=lines;m->pixel_height=h;return UMI_STATUS_OK;}
/*
 * Provide the editor wb minimap model y for line operation used by this module and its
 * client applications.
 */
int32_t umi_editor_wb_minimap_model_y_for_line(const UmiEditorWbMinimapModel *m,uint32_t line){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||m->document_lines==0U||line==0U)return -1;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(line>m->document_lines)line=m->document_lines;uint64_t numerator=(uint64_t)(line-1U)*(uint64_t)(uint32_t)m->pixel_height;return (int32_t)(numerator/(uint64_t)m->document_lines);}
