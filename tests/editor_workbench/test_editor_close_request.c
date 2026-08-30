/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_close_request.c
 *
 * PURPOSE:
 *   Implement the test editor close request behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_close_request.h"
int main(void){ UmiEditorWbEditorCloseRequest s; if(umi_editor_wb_editor_close_request_init(&s,"a.c","main")!=UMI_STATUS_OK)return 1; if(!umi_editor_wb_editor_close_request_valid(&s))return 2; return 0; }
