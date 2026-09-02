/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_model_context_window.c
 *
 * PURPOSE:
 *   Implement the test model context window behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/model_context_window.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiAiDevModelContextWindow b; umi_ai_dev_model_context_window_init(&b, 100U); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (umi_ai_dev_model_context_window_reserve(&b, 25U)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if (umi_ai_dev_model_context_window_utilisation(&b)!=25U) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if (umi_ai_dev_model_context_window_reserve(&b, 80U)!=UMI_STATUS_CAPACITY_EXCEEDED) return 3; /* Preserve the original failure result so the caller can respond to the correct cause. */ if (umi_ai_dev_model_context_window_consume(&b, 20U)!=UMI_STATUS_OK) return 4; return 0; }
