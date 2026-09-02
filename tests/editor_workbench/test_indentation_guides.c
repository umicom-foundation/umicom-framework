/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_indentation_guides.c
 *
 * PURPOSE:
 *   Implement the test indentation guides behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/indentation_guides.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ /* Apply this branch only when its contract condition is satisfied. */ if(umi_editor_wb_indentation_guides_depth(12U,4U)!=3U)return 1; /* Apply this branch only when its contract condition is satisfied. */ if(umi_editor_wb_indentation_guides_depth(4U,0U)!=0U)return 2; return 0; }
