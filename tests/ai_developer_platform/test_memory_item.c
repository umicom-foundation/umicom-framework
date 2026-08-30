/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_platform/test_memory_item.c
 *
 * PURPOSE:
 *   Implement the test memory item behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/memory_item.h"
int main(void) {
    UmiAiDevMemoryItem v;
    umi_ai_dev_memory_item_init(&v);
    if (umi_ai_dev_memory_item_configure(&v, "item", "Item", 12U, 3U) != UMI_STATUS_OK) return 1;
    if (umi_ai_dev_memory_item_validate(&v) != UMI_STATUS_OK) return 2;
    if (umi_ai_dev_memory_item_evidence_score(&v, 50U) != 62U) return 3;
    return 0;
}
