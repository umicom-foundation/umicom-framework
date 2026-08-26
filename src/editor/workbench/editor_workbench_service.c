/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_workbench_service.c
 *
 * PURPOSE:
 *   Aggregate editor-workbench readiness and lifecycle counters for thin IDE frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_workbench_service.h"

#include <string.h>
void umi_editor_wb_editor_workbench_service_init(UmiEditorWbEditorWorkbenchService *s){if(s!=NULL)memset(s,0,sizeof *s);}
int umi_editor_wb_editor_workbench_service_ready(const UmiEditorWbEditorWorkbenchService *s){return s!=NULL&&s->editor_ready&&s->ui_ready&&s->navigation_ready;}
void umi_editor_wb_editor_workbench_service_record_open(UmiEditorWbEditorWorkbenchService *s){if(s!=NULL)s->opened++;}
void umi_editor_wb_editor_workbench_service_record_close(UmiEditorWbEditorWorkbenchService *s){if(s!=NULL)s->closed++;}
void umi_editor_wb_editor_workbench_service_record_restore(UmiEditorWbEditorWorkbenchService *s){if(s!=NULL)s->restored++;}
