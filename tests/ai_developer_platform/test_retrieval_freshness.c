/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_retrieval_freshness.c
 *
 * PURPOSE:
 *   Implement the test retrieval freshness behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/retrieval_freshness.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiAiDevRetrievalFreshnessScore a={80U,60U,100U,0U}, b={40U,40U,40U,0U}; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (umi_ai_dev_retrieval_freshness_compute(&a) <= umi_ai_dev_retrieval_freshness_compute(&b)) return 1; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (!umi_ai_dev_retrieval_freshness_prefer(&a,&b)) return 2; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (umi_ai_dev_retrieval_freshness_weighted(100U,0U,50U)!=50U) return 3; return 0; }
