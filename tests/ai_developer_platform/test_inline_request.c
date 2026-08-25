#include "umicom/ai/developer_platform/inline_request.h"
int main(void) {
    UmiAiDevInlineRequest v;
    umi_ai_dev_inline_request_init(&v);
    if (umi_ai_dev_inline_request_configure(&v, "item", "Item", 12U, 3U) != UMI_STATUS_OK) return 1;
    if (umi_ai_dev_inline_request_validate(&v) != UMI_STATUS_OK) return 2;
    if (umi_ai_dev_inline_request_evidence_score(&v, 50U) != 62U) return 3;
    return 0;
}
