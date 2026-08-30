/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_retrieval_diversity.c
 *
 * PURPOSE:
 *   Implement the test retrieval diversity behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/retrieval_diversity.h"
int main(void) { UmiAiDevRetrievalDiversityScore a={80U,60U,100U,0U}, b={40U,40U,40U,0U}; if (umi_ai_dev_retrieval_diversity_compute(&a) <= umi_ai_dev_retrieval_diversity_compute(&b)) return 1; if (!umi_ai_dev_retrieval_diversity_prefer(&a,&b)) return 2; if (umi_ai_dev_retrieval_diversity_weighted(100U,0U,50U)!=50U) return 3; return 0; }
