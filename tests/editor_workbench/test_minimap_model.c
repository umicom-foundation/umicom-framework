/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_minimap_model.c
 *
 * PURPOSE:
 *   Implement the test minimap model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/minimap_model.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbMinimapModel m; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_minimap_model_init(&m,100U,200)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_minimap_model_y_for_line(&m,51U)!=100)return 2; return 0; }
