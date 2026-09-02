/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_group_registry.c
 *
 * PURPOSE:
 *   Implement the test editor group registry behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_group_registry.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbEditorGroupRegistry s; umi_editor_wb_editor_group_registry_init(&s); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_editor_wb_editor_group_registry_add(&s,"one")!=UMI_STATUS_OK)return 1; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!umi_editor_wb_editor_group_registry_contains(&s,"one"))return 2; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_editor_wb_editor_group_registry_remove(&s,"one")!=UMI_STATUS_OK||s.count!=0U)return 3; return 0; }
