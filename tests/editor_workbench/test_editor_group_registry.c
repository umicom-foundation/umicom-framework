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
int main(void){ UmiEditorWbEditorGroupRegistry s; umi_editor_wb_editor_group_registry_init(&s); if(umi_editor_wb_editor_group_registry_add(&s,"one")!=UMI_STATUS_OK)return 1; if(!umi_editor_wb_editor_group_registry_contains(&s,"one"))return 2; if(umi_editor_wb_editor_group_registry_remove(&s,"one")!=UMI_STATUS_OK||s.count!=0U)return 3; return 0; }
