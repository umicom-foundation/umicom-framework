/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_hybrid_retrieval.c
 *
 * PURPOSE:
 *   Implement the test hybrid retrieval behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/hybrid_retrieval.h"
int main(void) { UmiAiDevHybridRetrievalScore a={80U,60U,100U,0U}, b={40U,40U,40U,0U}; if (umi_ai_dev_hybrid_retrieval_compute(&a) <= umi_ai_dev_hybrid_retrieval_compute(&b)) return 1; if (!umi_ai_dev_hybrid_retrieval_prefer(&a,&b)) return 2; if (umi_ai_dev_hybrid_retrieval_weighted(100U,0U,50U)!=50U) return 3; return 0; }
