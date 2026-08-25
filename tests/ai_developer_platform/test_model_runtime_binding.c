#include "umicom/ai/developer_platform/model_runtime_binding.h"
int main(void) {
    UmiAiDevModelRuntimeBinding v;
    umi_ai_dev_model_runtime_binding_init(&v);
    if (umi_ai_dev_model_runtime_binding_configure(&v, "item", "Item", 12U, 3U) != UMI_STATUS_OK) return 1;
    if (umi_ai_dev_model_runtime_binding_validate(&v) != UMI_STATUS_OK) return 2;
    if (umi_ai_dev_model_runtime_binding_evidence_score(&v, 50U) != 62U) return 3;
    return 0;
}
