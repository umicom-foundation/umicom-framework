/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_location.c
 *
 * PURPOSE:
 *   Implement the test editor location behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_location.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbEditorLocation l; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_location_init(&l,"a.c",3U,4U)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_wb_editor_location_valid(&l))return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_location_init(&l,"a.c",0U,1U)!=UMI_STATUS_INVALID_ARGUMENT)return 3; return 0; }
