/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_folding_projection.c
 *
 * PURPOSE:
 *   Implement the test folding projection behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/folding_projection.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbFoldingProjection p; umi_editor_wb_folding_projection_init(&p); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_folding_projection_add(&p,3U,8U)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_wb_folding_projection_line_hidden(&p,4U)||umi_editor_wb_folding_projection_line_hidden(&p,3U))return 2; return 0; }
