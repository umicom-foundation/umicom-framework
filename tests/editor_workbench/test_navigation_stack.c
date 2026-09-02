/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor_workbench/test_navigation_stack.c
 *
 * PURPOSE:
 *   Implement the test navigation stack behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/navigation_stack.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEditorWbNavigationStack s; UmiEditorWbEditorLocation a,b; umi_editor_wb_navigation_stack_init(&s); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_location_init(&a,"a.c",1U,1U)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_editor_location_init(&b,"b.c",2U,1U)!=UMI_STATUS_OK)return 2; (void)umi_editor_wb_navigation_stack_push(&s,&a); (void)umi_editor_wb_navigation_stack_push(&s,&b); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_navigation_stack_back(&s)==0)return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_editor_wb_navigation_stack_forward(&s)==0)return 4; return 0; }
