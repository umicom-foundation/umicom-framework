/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_context_pack.c
 *
 * PURPOSE:
 *   Implement the test context pack behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/context_pack.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiAiDevContextPack c; umi_ai_dev_context_pack_init(&c); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ai_dev_context_pack_add(&c,"a")!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ai_dev_context_pack_add(&c,"a")!=UMI_STATUS_ALREADY_EXISTS) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ai_dev_context_pack_contains(&c,"a")) return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ai_dev_context_pack_remove(&c,"a")!=UMI_STATUS_OK) return 4; return 0; }
