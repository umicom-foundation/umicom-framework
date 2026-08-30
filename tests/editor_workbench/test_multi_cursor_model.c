/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_multi_cursor_model.c
 *
 * PURPOSE:
 *   Implement the test multi cursor model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/multi_cursor_model.h"
int main(void){ UmiEditorWbMultiCursorModel m; UmiEditorWbPosition p={1U,1U}; umi_editor_wb_multi_cursor_model_init(&m); if(umi_editor_wb_multi_cursor_model_add(&m,p)!=UMI_STATUS_OK)return 1; if(umi_editor_wb_multi_cursor_model_add(&m,p)!=UMI_STATUS_ALREADY_EXISTS)return 2; if(umi_editor_wb_multi_cursor_model_remove(&m,p)!=UMI_STATUS_OK)return 3; return 0; }
