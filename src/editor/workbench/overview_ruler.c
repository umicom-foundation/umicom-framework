/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/overview_ruler.c
 *
 * PURPOSE:
 *   Maintain diagnostics/bookmark/search markers for an editor overview ruler.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/overview_ruler.h"

#include <string.h>
void umi_editor_wb_overview_ruler_init(UmiEditorWbOverviewRuler *s){if(s!=NULL)memset(s,0,sizeof *s);}
UmiStatus umi_editor_wb_overview_ruler_add(UmiEditorWbOverviewRuler *s,uint32_t line,UmiEditorWbSeverity sev){if(s==NULL||line==0U)return UMI_STATUS_INVALID_ARGUMENT;if(s->count>=UMI_EDITOR_WB_MAX_SEGMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;s->markers[s->count].line=line;s->markers[s->count].severity=sev;s->count++;return UMI_STATUS_OK;}
