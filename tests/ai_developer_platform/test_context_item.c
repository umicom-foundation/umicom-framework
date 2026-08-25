#include "umicom/ai/developer_platform/context_item.h"
int main(void) {
    UmiAiDevContextItem v;
    umi_ai_dev_context_item_init(&v);
    if (umi_ai_dev_context_item_configure(&v, "item", "Item", 12U, 3U) != UMI_STATUS_OK) return 1;
    if (umi_ai_dev_context_item_validate(&v) != UMI_STATUS_OK) return 2;
    if (umi_ai_dev_context_item_evidence_score(&v, 50U) != 62U) return 3;
    return 0;
}
