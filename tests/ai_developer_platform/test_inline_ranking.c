/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_inline_ranking.c
 *
 * PURPOSE:
 *   Implement the test inline ranking behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/inline_ranking.h"
int main(void) { UmiAiDevInlineRankingScore a={80U,60U,100U,0U}, b={40U,40U,40U,0U}; if (umi_ai_dev_inline_ranking_compute(&a) <= umi_ai_dev_inline_ranking_compute(&b)) return 1; if (!umi_ai_dev_inline_ranking_prefer(&a,&b)) return 2; if (umi_ai_dev_inline_ranking_weighted(100U,0U,50U)!=50U) return 3; return 0; }
