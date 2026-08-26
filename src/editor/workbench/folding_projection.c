/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/folding_projection.c
 *
 * PURPOSE:
 *   Project fold ranges into line-visibility decisions without owning language parsing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/folding_projection.h"

#include <string.h>
void umi_editor_wb_folding_projection_init(UmiEditorWbFoldingProjection *p){if(p!=NULL)memset(p,0,sizeof *p);}
UmiStatus umi_editor_wb_folding_projection_add(UmiEditorWbFoldingProjection *p,uint32_t first,uint32_t last){if(p==NULL||first==0U||last<=first)return UMI_STATUS_INVALID_ARGUMENT;if(p->count>=UMI_EDITOR_WB_MAX_SEGMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;p->folds[p->count].start.line=first;p->folds[p->count].start.column=1U;p->folds[p->count].end.line=last;p->folds[p->count].end.column=1U;p->count++;return UMI_STATUS_OK;}
int umi_editor_wb_folding_projection_line_hidden(const UmiEditorWbFoldingProjection *p,uint32_t line){size_t i;if(p==NULL)return 0;for(i=0U;i<p->count;i++)if(line>p->folds[i].start.line&&line<=p->folds[i].end.line)return 1;return 0;}
