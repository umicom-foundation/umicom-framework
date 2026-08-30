/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_repository_document.c
 *
 * PURPOSE:
 *   Implement the test repository document behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/repository_document.h"
int main(void) {
    UmiAiDevRepositoryDocument v;
    umi_ai_dev_repository_document_init(&v);
    if (umi_ai_dev_repository_document_configure(&v, "item", "Item", 12U, 3U) != UMI_STATUS_OK) return 1;
    if (umi_ai_dev_repository_document_validate(&v) != UMI_STATUS_OK) return 2;
    if (umi_ai_dev_repository_document_evidence_score(&v, 50U) != 62U) return 3;
    return 0;
}
