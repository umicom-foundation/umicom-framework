/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_commit_context.c
 *
 * PURPOSE:
 *   Implement the test commit context behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/commit_context.h"
int main(void) {
    UmiAiDevCommitContext v;
    umi_ai_dev_commit_context_init(&v);
    if (umi_ai_dev_commit_context_configure(&v, "item", "Item", 12U, 3U) != UMI_STATUS_OK) return 1;
    if (umi_ai_dev_commit_context_validate(&v) != UMI_STATUS_OK) return 2;
    if (umi_ai_dev_commit_context_evidence_score(&v, 50U) != 62U) return 3;
    return 0;
}
