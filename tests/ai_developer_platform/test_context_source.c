/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_context_source.c
 *
 * PURPOSE:
 *   Implement the test context source behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/context_source.h"
int main(void) {
    UmiAiDevContextSource v;
    umi_ai_dev_context_source_init(&v);
    if (umi_ai_dev_context_source_configure(&v, "item", "Item", 12U, 3U) != UMI_STATUS_OK) return 1;
    if (umi_ai_dev_context_source_validate(&v) != UMI_STATUS_OK) return 2;
    if (umi_ai_dev_context_source_evidence_score(&v, 50U) != 62U) return 3;
    return 0;
}
