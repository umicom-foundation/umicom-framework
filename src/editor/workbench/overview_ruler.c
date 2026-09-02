/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/overview_ruler.c
 *
 * PURPOSE:
 *   Maintain diagnostics/bookmark/search markers for an editor overview ruler.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/overview_ruler.h"

#include <string.h>
/*
 * Initialise editor wb overview ruler from caller-provided values so later operations
 * receive a known state.
 */
void umi_editor_wb_overview_ruler_init(UmiEditorWbOverviewRuler *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)memset(s,0,sizeof *s);}
/*
 * Add editor wb overview ruler only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_editor_wb_overview_ruler_add(UmiEditorWbOverviewRuler *s,uint32_t line,UmiEditorWbSeverity sev){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||line==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=UMI_EDITOR_WB_MAX_SEGMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;s->markers[s->count].line=line;s->markers[s->count].severity=sev;s->count++;return UMI_STATUS_OK;}
