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
int main(void){ UmiEditorWbTabSortKey a={true,false,"b"},b={false,true,"a"}; if(umi_editor_wb_tab_sort_compare(&a,&b)>=0)return 1; return 0; }
