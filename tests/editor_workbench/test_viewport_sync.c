/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_viewport_sync.c
 *
 * PURPOSE:
 *   Implement the test viewport sync behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/viewport_sync.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbViewportSync s={true,false,0,0,0,8}; umi_editor_wb_viewport_sync_apply(&s,4.0,5.0); /* Apply this branch only when its contract condition is satisfied. */ if(s.follower_horizontal!=4.0||s.follower_vertical!=8.0)return 1; return 0; }
