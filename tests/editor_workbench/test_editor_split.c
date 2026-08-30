/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_split.c
 *
 * PURPOSE:
 *   Implement the test editor split behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_split.h"
int main(void){ UmiEditorWbEditorSplit s; if(umi_editor_wb_editor_split_init(&s,"s",UMI_EDITOR_WB_HORIZONTAL,0.5)!=UMI_STATUS_OK)return 1; if(umi_editor_wb_editor_split_set_ratio(&s,0.95)!=UMI_STATUS_INVALID_ARGUMENT)return 2; if(!umi_editor_wb_editor_split_valid(&s))return 3; return 0; }
