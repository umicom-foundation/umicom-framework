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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbDirtyDocumentState s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_dirty_document_state_init(&s,"item",false)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_dirty_document_state_set(&s,true)!=UMI_STATUS_OK)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_wb_dirty_document_state_valid(&s)||!s.enabled)return 3; return 0; }
