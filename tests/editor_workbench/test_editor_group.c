/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_group.c
 *
 * PURPOSE:
 *   Implement the test editor group behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_group.h"
int main(void){ UmiEditorWbEditorGroup s; if(umi_editor_wb_editor_group_init(&s,"id","")!=UMI_STATUS_OK)return 1; if(umi_editor_wb_editor_group_set_count(&s,3U,1U)!=UMI_STATUS_OK)return 2; if(!umi_editor_wb_editor_group_valid(&s)||s.active_index!=1U)return 3; return 0; }
