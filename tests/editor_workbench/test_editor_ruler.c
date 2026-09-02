/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_editor_ruler.c
 *
 * PURPOSE:
 *   Implement the test editor ruler behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_ruler.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbEditorRuler s; umi_editor_wb_editor_ruler_init(&s); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_ruler_append(&s,"one",1U)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_ruler_append(&s,"two",2U)!=UMI_STATUS_OK)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_ruler_activate(&s,1U)!=UMI_STATUS_OK||s.active_index!=1U)return 3; return 0; }
