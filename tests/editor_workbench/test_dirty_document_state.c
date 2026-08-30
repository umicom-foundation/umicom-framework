/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_dirty_document_state.c
 *
 * PURPOSE:
 *   Implement the test dirty document state behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/dirty_document_state.h"
int main(void){ UmiEditorWbDirtyDocumentState s; if(umi_editor_wb_dirty_document_state_init(&s,"item",false)!=UMI_STATUS_OK)return 1; if(umi_editor_wb_dirty_document_state_set(&s,true)!=UMI_STATUS_OK)return 2; if(!umi_editor_wb_dirty_document_state_valid(&s)||!s.enabled)return 3; return 0; }
