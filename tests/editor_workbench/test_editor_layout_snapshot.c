/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_layout_snapshot.c
 *
 * PURPOSE:
 *   Implement the test editor layout snapshot behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_layout_snapshot.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbEditorLayoutSnapshot s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_layout_snapshot_capture(&s,"active",5U,2U,9U)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_wb_editor_layout_snapshot_valid(&s)||s.fingerprint==0U)return 2; return 0; }
