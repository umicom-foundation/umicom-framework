/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_scroll_state.c
 *
 * PURPOSE:
 *   Implement the test scroll state behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/scroll_state.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbScrollState s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_scroll_state_init(&s,100.0,200.0)!=UMI_STATUS_OK)return 1; (void)umi_editor_wb_scroll_state_set(&s,120.0,-5.0); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s.horizontal!=100.0||s.vertical!=0.0)return 2; return 0; }
