/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_navigation_restore.c
 *
 * PURPOSE:
 *   Implement the test navigation restore behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/navigation_restore.h"
int main(void){ UmiEditorWbNavigationRestore s; if(umi_editor_wb_navigation_restore_init(&s,"id","text")!=UMI_STATUS_OK)return 1; if(umi_editor_wb_navigation_restore_set_values(&s,3U,4U,true)!=UMI_STATUS_OK)return 2; if(!umi_editor_wb_navigation_restore_valid(&s)||s.primary!=3U)return 3; return 0; }
