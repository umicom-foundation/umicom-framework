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
int main(void){ UmiEditorWbTabOverflow s; if(umi_editor_wb_tab_overflow_resolve(&s,10U,500,100)!=UMI_STATUS_OK)return 1; if(s.visible_tabs!=5U||s.overflow_tabs!=5U)return 2; return 0; }
