/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_workspace_memory.c
 *
 * PURPOSE:
 *   Implement the test workspace memory behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/workspace_memory.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiAiDevWorkspaceMemory c; umi_ai_dev_workspace_memory_init(&c); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ai_dev_workspace_memory_add(&c,"a")!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ai_dev_workspace_memory_add(&c,"a")!=UMI_STATUS_ALREADY_EXISTS) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ai_dev_workspace_memory_contains(&c,"a")) return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ai_dev_workspace_memory_remove(&c,"a")!=UMI_STATUS_OK) return 4; return 0; }
