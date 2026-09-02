/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_lexical_query.c
 *
 * PURPOSE:
 *   Implement the test lexical query behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/lexical_query.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiAiDevLexicalQueryScore a={80U,60U,100U,0U}, b={40U,40U,40U,0U}; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (umi_ai_dev_lexical_query_compute(&a) <= umi_ai_dev_lexical_query_compute(&b)) return 1; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (!umi_ai_dev_lexical_query_prefer(&a,&b)) return 2; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (umi_ai_dev_lexical_query_weighted(100U,0U,50U)!=50U) return 3; return 0; }
