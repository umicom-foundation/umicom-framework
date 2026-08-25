#include "umicom/ai/developer_platform/context_provenance.h"
int main(void) {
    UmiAiDevContextProvenance v;
    umi_ai_dev_context_provenance_init(&v);
    if (umi_ai_dev_context_provenance_configure(&v, "item", "Item", 12U, 3U) != UMI_STATUS_OK) return 1;
    if (umi_ai_dev_context_provenance_validate(&v) != UMI_STATUS_OK) return 2;
    if (umi_ai_dev_context_provenance_evidence_score(&v, 50U) != 62U) return 3;
    return 0;
}
