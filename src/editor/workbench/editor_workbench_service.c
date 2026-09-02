/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_workbench_service.c
 *
 * PURPOSE:
 *   Aggregate editor-workbench readiness and lifecycle counters for thin IDE frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_workbench_service.h"

#include <string.h>
/*
 * Initialise editor wb editor workbench service from caller-provided values so later
 * operations receive a known state.
 */
void umi_editor_wb_editor_workbench_service_init(UmiEditorWbEditorWorkbenchService *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)memset(s,0,sizeof *s);}
/*
 * Provide the editor wb editor workbench service ready operation used by this module and
 * its client applications.
 */
int umi_editor_wb_editor_workbench_service_ready(const UmiEditorWbEditorWorkbenchService *s){return s!=NULL&&s->editor_ready&&s->ui_ready&&s->navigation_ready;}
/*
 * Provide the editor wb editor workbench service record open operation used by this module
 * and its client applications.
 */
void umi_editor_wb_editor_workbench_service_record_open(UmiEditorWbEditorWorkbenchService *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)s->opened++;}
/*
 * Provide the editor wb editor workbench service record close operation used by this
 * module and its client applications.
 */
void umi_editor_wb_editor_workbench_service_record_close(UmiEditorWbEditorWorkbenchService *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)s->closed++;}
/*
 * Provide the editor wb editor workbench service record restore operation used by this
 * module and its client applications.
 */
void umi_editor_wb_editor_workbench_service_record_restore(UmiEditorWbEditorWorkbenchService *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)s->restored++;}
