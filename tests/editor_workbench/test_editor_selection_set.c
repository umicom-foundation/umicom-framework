/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_selection_set.c
 *
 * PURPOSE:
 *   Implement the test editor selection set behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_selection_set.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbEditorSelectionSet s; UmiEditorWbRange r={{4U,1U},{2U,1U}}; umi_editor_wb_editor_selection_set_init(&s); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_selection_set_add(&s,r,true)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s.ranges[0].start.line!=2U||s.primary_index!=0U)return 2; return 0; }
