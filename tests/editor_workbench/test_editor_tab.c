/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_tab.c
 *
 * PURPOSE:
 *   Implement the test editor tab behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_tab.h"
int main(void){ UmiEditorWbEditorTab s; if(umi_editor_wb_editor_tab_init(&s,"id","")!=UMI_STATUS_OK)return 1; if(umi_editor_wb_editor_tab_set_count(&s,3U,1U)!=UMI_STATUS_OK)return 2; if(!umi_editor_wb_editor_tab_valid(&s)||s.active_index!=1U)return 3; return 0; }
