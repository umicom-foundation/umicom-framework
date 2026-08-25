#include "umicom/ai/developer_platform/symbol_context.h"
int main(void) {
    UmiAiDevSymbolContext v;
    umi_ai_dev_symbol_context_init(&v);
    if (umi_ai_dev_symbol_context_configure(&v, "item", "Item", 12U, 3U) != UMI_STATUS_OK) return 1;
    if (umi_ai_dev_symbol_context_validate(&v) != UMI_STATUS_OK) return 2;
    if (umi_ai_dev_symbol_context_evidence_score(&v, 50U) != 62U) return 3;
    return 0;
}
