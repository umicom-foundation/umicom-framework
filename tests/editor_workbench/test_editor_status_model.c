/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_status_model.c
 *
 * PURPOSE:
 *   Implement the test editor status model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_status_model.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbEditorStatusModel s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_status_model_init(&s,"C","UTF-8")!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_status_model_set_position(&s,7U,3U)!=UMI_STATUS_OK||s.line!=7U)return 2; return 0; }
