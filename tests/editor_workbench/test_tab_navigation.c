/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_tab_navigation.c
 *
 * PURPOSE:
 *   Implement the test tab navigation behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/tab_navigation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ /* Apply this branch only when its contract condition is satisfied. */ if(umi_editor_wb_tab_navigation_next(2U,3U,1,true)!=0U)return 1; /* Apply this branch only when its contract condition is satisfied. */ if(umi_editor_wb_tab_navigation_next(0U,3U,-1,true)!=2U)return 2; return 0; }
