/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_context_cache.c
 *
 * PURPOSE:
 *   Implement the test context cache behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/context_cache.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiAiDevContextCache c; uint32_t s=0U; umi_ai_dev_context_cache_init(&c); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ai_dev_context_cache_put(&c,7U,2U,91U)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ai_dev_context_cache_get(&c,7U,2U,&s)||s!=91U)return 2; umi_ai_dev_context_cache_invalidate_before(&c,3U); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ai_dev_context_cache_get(&c,7U,2U,&s))return 3; return 0; }
