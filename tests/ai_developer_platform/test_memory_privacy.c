/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_memory_privacy.c
 *
 * PURPOSE:
 *   Implement the test memory privacy behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/memory_privacy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiAiDevMemoryPrivacy p; umi_ai_dev_memory_privacy_configure(&p, 7U, 4U, UMI_AI_DEV_RISK_HIGH, UMI_AI_DEV_LOCALITY_ANY, 1); /* Apply this branch only when its contract condition is satisfied. */ if (!umi_ai_dev_memory_privacy_allows(&p, 1U, UMI_AI_DEV_RISK_LOW, UMI_AI_DEV_LOCALITY_LOCAL)) return 1; /* Apply this branch only when its contract condition is satisfied. */ if (umi_ai_dev_memory_privacy_allows(&p, 4U, UMI_AI_DEV_RISK_LOW, UMI_AI_DEV_LOCALITY_LOCAL)) return 2; /* Apply this branch only when its contract condition is satisfied. */ if (!umi_ai_dev_memory_privacy_approval_required(&p, UMI_AI_DEV_RISK_HIGH)) return 3; return 0; }
