/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_context_ranking.c
 *
 * PURPOSE:
 *   Implement the test context ranking behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/context_ranking.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiAiDevContextRankingScore a={80U,60U,100U,0U}, b={40U,40U,40U,0U}; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (umi_ai_dev_context_ranking_compute(&a) <= umi_ai_dev_context_ranking_compute(&b)) return 1; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (!umi_ai_dev_context_ranking_prefer(&a,&b)) return 2; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (umi_ai_dev_context_ranking_weighted(100U,0U,50U)!=50U) return 3; return 0; }
