/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_code_map.c
 *
 * PURPOSE:
 *   Implement the test code map behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/code_map.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbCodeMap m; UmiEditorWbCodeMapSegment s; umi_editor_wb_code_map_init(&m); (void)umi_editor_wb_code_map_segment_init(&s,"s",1U,5U,UMI_EDITOR_WB_INFO); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_code_map_add(&m,&s)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_code_map_find_line(&m,3U)==0)return 2; return 0; }
