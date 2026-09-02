/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_inline_session.c
 *
 * PURPOSE:
 *   Implement the test inline session behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/inline_session.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiAiDevInlineSession v; umi_ai_dev_inline_session_init(&v); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (umi_ai_dev_inline_session_record(&v, 1) != UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if (!umi_ai_dev_inline_session_ready(&v)) return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if (umi_ai_dev_inline_session_health_score(&v) != 100U) return 3; return 0; }
