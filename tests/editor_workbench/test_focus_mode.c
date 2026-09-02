/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_focus_mode.c
 *
 * PURPOSE:
 *   Implement the test focus mode behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/focus_mode.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbFocusMode s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_focus_mode_init(&s,"item",false)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_focus_mode_set(&s,true)!=UMI_STATUS_OK)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_wb_focus_mode_valid(&s)||!s.enabled)return 3; return 0; }
