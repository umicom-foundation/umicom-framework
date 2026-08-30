/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_active_editor.c
 *
 * PURPOSE:
 *   Implement the test active editor behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/active_editor.h"
int main(void){ UmiEditorWbActiveEditor s={{0},{0},0U}; if(umi_editor_wb_active_editor_set(&s,"item","group")!=UMI_STATUS_OK)return 1; umi_editor_wb_active_editor_clear(&s); if(s.item_id[0]!='\0')return 2; return 0; }
