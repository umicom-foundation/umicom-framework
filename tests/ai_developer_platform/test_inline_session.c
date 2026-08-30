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
int main(void) { UmiAiDevInlineSession v; umi_ai_dev_inline_session_init(&v); if (umi_ai_dev_inline_session_record(&v, 1) != UMI_STATUS_OK) return 1; if (!umi_ai_dev_inline_session_ready(&v)) return 2; if (umi_ai_dev_inline_session_health_score(&v) != 100U) return 3; return 0; }
