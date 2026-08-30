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
int main(void){ UmiEditorWbEditorSearchMatch m; UmiEditorWbRange r={{3U,1U},{2U,1U}}; if(umi_editor_wb_editor_search_match_init(&m,r,true)!=UMI_STATUS_OK)return 1; if(m.range.start.line!=2U||!m.selected)return 2; return 0; }
