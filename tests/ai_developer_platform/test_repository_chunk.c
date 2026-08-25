#include "umicom/ai/developer_platform/repository_chunk.h"
int main(void) {
    UmiAiDevRepositoryChunk v;
    umi_ai_dev_repository_chunk_init(&v);
    if (umi_ai_dev_repository_chunk_configure(&v, "item", "Item", 12U, 3U) != UMI_STATUS_OK) return 1;
    if (umi_ai_dev_repository_chunk_validate(&v) != UMI_STATUS_OK) return 2;
    if (umi_ai_dev_repository_chunk_evidence_score(&v, 50U) != 62U) return 3;
    return 0;
}
