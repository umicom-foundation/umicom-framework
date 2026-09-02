/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_workbench_service.c
 *
 * PURPOSE:
 *   Implement the test editor workbench service behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_workbench_service.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbEditorWorkbenchService s; umi_editor_wb_editor_workbench_service_init(&s); s.editor_ready=true;s.ui_ready=true;s.navigation_ready=true; umi_editor_wb_editor_workbench_service_record_open(&s); umi_editor_wb_editor_workbench_service_record_restore(&s); /* Apply this operation only while the related capability or state is available. */ if(!umi_editor_wb_editor_workbench_service_ready(&s)||s.opened!=1U||s.restored!=1U)return 1; return 0; }
