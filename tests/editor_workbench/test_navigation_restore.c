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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbNavigationRestore s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_navigation_restore_init(&s,"id","text")!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_navigation_restore_set_values(&s,3U,4U,true)!=UMI_STATUS_OK)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_wb_navigation_restore_valid(&s)||s.primary!=3U)return 3; return 0; }
