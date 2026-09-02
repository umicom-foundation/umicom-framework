/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_types.c
 *
 * PURPOSE:
 *   Implement the test types behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/types.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ char b[8]; UmiEditorWbRange r={ {4U,2U},{2U,1U} }; r=umi_editor_wb_range_normalize(r); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_copy_text(b,sizeof b,"editor")!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(r.start.line!=2U||r.end.line!=4U)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_editor_wb_id_valid("main"))return 3; return 0; }
