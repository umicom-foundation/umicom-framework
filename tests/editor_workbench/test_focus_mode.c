/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_focus_mode.c
 *
 * PURPOSE:
 *   Implement the test focus mode behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/focus_mode.h"
int main(void){ UmiEditorWbFocusMode s; if(umi_editor_wb_focus_mode_init(&s,"item",false)!=UMI_STATUS_OK)return 1; if(umi_editor_wb_focus_mode_set(&s,true)!=UMI_STATUS_OK)return 2; if(!umi_editor_wb_focus_mode_valid(&s)||!s.enabled)return 3; return 0; }
