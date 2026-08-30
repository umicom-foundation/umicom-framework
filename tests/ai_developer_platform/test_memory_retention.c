/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_memory_retention.c
 *
 * PURPOSE:
 *   Implement the test memory retention behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/memory_retention.h"
int main(void) { UmiAiDevMemoryRetention p; umi_ai_dev_memory_retention_configure(&p, 7U, 4U, UMI_AI_DEV_RISK_HIGH, UMI_AI_DEV_LOCALITY_ANY, 1); if (!umi_ai_dev_memory_retention_allows(&p, 1U, UMI_AI_DEV_RISK_LOW, UMI_AI_DEV_LOCALITY_LOCAL)) return 1; if (umi_ai_dev_memory_retention_allows(&p, 4U, UMI_AI_DEV_RISK_LOW, UMI_AI_DEV_LOCALITY_LOCAL)) return 2; if (!umi_ai_dev_memory_retention_approval_required(&p, UMI_AI_DEV_RISK_HIGH)) return 3; return 0; }
