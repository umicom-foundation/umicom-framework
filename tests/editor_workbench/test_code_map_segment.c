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
int main(void){ UmiEditorWbCodeMapSegment s; if(umi_editor_wb_code_map_segment_init(&s,"fn",10U,20U,UMI_EDITOR_WB_INFO)!=UMI_STATUS_OK)return 1; if(!umi_editor_wb_code_map_segment_contains(&s,15U)||umi_editor_wb_code_map_segment_contains(&s,21U))return 2; return 0; }
