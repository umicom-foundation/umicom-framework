/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_recently_closed_editor.c
 *
 * PURPOSE:
 *   Implement the test recently closed editor behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/recently_closed_editor.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbRecentlyClosedEditor h; char p[32]; umi_editor_wb_recently_closed_editor_init(&h); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_recently_closed_editor_push(&h,"a.c")!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_recently_closed_editor_push(&h,"b.c")!=UMI_STATUS_OK)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_recently_closed_editor_pop(&h,p,sizeof p)!=UMI_STATUS_OK)return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(p[0]!='b')return 4; return 0; }
