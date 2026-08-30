/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_repository_change.c
 *
 * PURPOSE:
 *   Implement the test repository change behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/repository_change.h"
int main(void) { UmiAiDevRepositoryChange v; umi_ai_dev_repository_change_init(&v); if (umi_ai_dev_repository_change_record(&v, 1) != UMI_STATUS_OK) return 1; if (!umi_ai_dev_repository_change_ready(&v)) return 2; if (umi_ai_dev_repository_change_health_score(&v) != 100U) return 3; return 0; }
