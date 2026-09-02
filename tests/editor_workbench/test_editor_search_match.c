/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_search_match.c
 *
 * PURPOSE:
 *   Implement the test editor search match behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_search_match.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbEditorSearchMatch m; UmiEditorWbRange r={{3U,1U},{2U,1U}}; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_search_match_init(&m,r,true)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(m.range.start.line!=2U||!m.selected)return 2; return 0; }
