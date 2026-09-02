/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_item_registry.c
 *
 * PURPOSE:
 *   Implement the test editor item registry behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_item_registry.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbEditorItemRegistry r; UmiEditorWbEditorItem x; umi_editor_wb_editor_item_registry_init(&r); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_editor_wb_editor_item_init(&x,"e","a.c",UMI_EDITOR_WB_OPEN_NORMAL)!=UMI_STATUS_OK)return 1; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_editor_wb_editor_item_registry_upsert(&r,&x)!=UMI_STATUS_OK)return 2; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_editor_wb_editor_item_registry_find(&r,"e")==0)return 3; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_editor_wb_editor_item_registry_remove(&r,"e")!=UMI_STATUS_OK||r.count!=0U)return 4; return 0; }
