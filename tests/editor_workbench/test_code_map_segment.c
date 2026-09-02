/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_code_map_segment.c
 *
 * PURPOSE:
 *   Implement the test code map segment behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/code_map_segment.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbCodeMapSegment s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_code_map_segment_init(&s,"fn",10U,20U,UMI_EDITOR_WB_INFO)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_wb_code_map_segment_contains(&s,15U)||umi_editor_wb_code_map_segment_contains(&s,21U))return 2; return 0; }
