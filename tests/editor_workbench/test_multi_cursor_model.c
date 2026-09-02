/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_multi_cursor_model.c
 *
 * PURPOSE:
 *   Implement the test multi cursor model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/multi_cursor_model.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbMultiCursorModel m; UmiEditorWbPosition p={1U,1U}; umi_editor_wb_multi_cursor_model_init(&m); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_multi_cursor_model_add(&m,p)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_multi_cursor_model_add(&m,p)!=UMI_STATUS_ALREADY_EXISTS)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_multi_cursor_model_remove(&m,p)!=UMI_STATUS_OK)return 3; return 0; }
