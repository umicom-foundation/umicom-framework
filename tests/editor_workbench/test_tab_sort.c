/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_tab_sort.c
 *
 * PURPOSE:
 *   Implement the test tab sort behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/tab_sort.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbTabSortKey a={true,false,"b"},b={false,true,"a"}; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_editor_wb_tab_sort_compare(&a,&b)>=0)return 1; return 0; }
