/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/folding_projection.c
 *
 * PURPOSE:
 *   Project fold ranges into line-visibility decisions without owning language parsing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/folding_projection.h"

#include <string.h>
/*
 * Initialise editor wb folding projection from caller-provided values so later operations
 * receive a known state.
 */
void umi_editor_wb_folding_projection_init(UmiEditorWbFoldingProjection *p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL)memset(p,0,sizeof *p);}
/*
 * Add editor wb folding projection only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_editor_wb_folding_projection_add(UmiEditorWbFoldingProjection *p,uint32_t first,uint32_t last){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||first==0U||last<=first)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->count>=UMI_EDITOR_WB_MAX_SEGMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;p->folds[p->count].start.line=first;p->folds[p->count].start.column=1U;p->folds[p->count].end.line=last;p->folds[p->count].end.column=1U;p->count++;return UMI_STATUS_OK;}
/*
 * Provide the editor wb folding projection line hidden operation used by this module and
 * its client applications.
 */
int umi_editor_wb_folding_projection_line_hidden(const UmiEditorWbFoldingProjection *p,uint32_t line){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<p->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(line>p->folds[i].start.line&&line<=p->folds[i].end.line)return 1;return 0;}
