/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_close_policy.c
 *
 * PURPOSE:
 *   Implement the test editor close policy behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_close_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbEditorClosePolicy p; umi_editor_wb_editor_close_policy_init(&p); /* Apply this branch only when its contract condition is satisfied. */ if(umi_editor_wb_editor_close_policy_evaluate(&p,true,false,false)!=UMI_EDITOR_WB_CLOSE_CONFIRM)return 1; /* Apply this branch only when its contract condition is satisfied. */ if(umi_editor_wb_editor_close_policy_evaluate(&p,false,true,false)!=UMI_EDITOR_WB_CLOSE_DENY)return 2; /* Apply this branch only when its contract condition is satisfied. */ if(umi_editor_wb_editor_close_policy_evaluate(&p,true,true,true)!=UMI_EDITOR_WB_CLOSE_ALLOW)return 3; return 0; }
