/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_synchronized_editors.c
 *
 * PURPOSE:
 *   Implement the test synchronized editors behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/synchronized_editors.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbSynchronizedEditors s; umi_editor_wb_synchronized_editors_init(&s); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_synchronized_editors_append(&s,"one",1U)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_synchronized_editors_append(&s,"two",2U)!=UMI_STATUS_OK)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_synchronized_editors_activate(&s,1U)!=UMI_STATUS_OK||s.active_index!=1U)return 3; return 0; }
