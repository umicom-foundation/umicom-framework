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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbEditorItem x; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_item_init(&x,"e1","a.c",UMI_EDITOR_WB_OPEN_PREVIEW)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_wb_editor_item_valid(&x))return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_item_set_dirty(&x,true)!=UMI_STATUS_OK||!x.dirty)return 3; return 0; }
