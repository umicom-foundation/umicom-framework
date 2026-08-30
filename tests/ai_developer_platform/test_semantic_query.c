/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_semantic_query.c
 *
 * PURPOSE:
 *   Implement the test semantic query behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/semantic_query.h"
int main(void) { UmiAiDevSemanticQueryScore a={80U,60U,100U,0U}, b={40U,40U,40U,0U}; if (umi_ai_dev_semantic_query_compute(&a) <= umi_ai_dev_semantic_query_compute(&b)) return 1; if (!umi_ai_dev_semantic_query_prefer(&a,&b)) return 2; if (umi_ai_dev_semantic_query_weighted(100U,0U,50U)!=50U) return 3; return 0; }
