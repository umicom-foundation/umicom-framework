/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_item.c
 *
 * PURPOSE:
 *   Implement the test editor item behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_item.h"
int main(void){ UmiEditorWbEditorItem x; if(umi_editor_wb_editor_item_init(&x,"e1","a.c",UMI_EDITOR_WB_OPEN_PREVIEW)!=UMI_STATUS_OK)return 1; if(!umi_editor_wb_editor_item_valid(&x))return 2; if(umi_editor_wb_editor_item_set_dirty(&x,true)!=UMI_STATUS_OK||!x.dirty)return 3; return 0; }
