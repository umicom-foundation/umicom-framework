/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_tab_overflow.c
 *
 * PURPOSE:
 *   Implement the test tab overflow behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/tab_overflow.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbTabOverflow s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_tab_overflow_resolve(&s,10U,500,100)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s.visible_tabs!=5U||s.overflow_tabs!=5U)return 2; return 0; }
