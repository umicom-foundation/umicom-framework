/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_context_catalogue.c
 *
 * PURPOSE:
 *   Implement the test context catalogue behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/context_catalogue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiAiDevContextCatalogue c; umi_ai_dev_context_catalogue_init(&c); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ai_dev_context_catalogue_upsert(&c,"x",5U,1U)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ai_dev_context_catalogue_find(&c,"x")==0) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ai_dev_context_catalogue_enabled_count(&c)!=1U) return 3; return 0; }
