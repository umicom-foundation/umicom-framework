#include "umicom/ai/developer_platform/repository_index_state.h"
int main(void) { UmiAiDevRepositoryIndexState v; umi_ai_dev_repository_index_state_init(&v); if (umi_ai_dev_repository_index_state_record(&v, 1) != UMI_STATUS_OK) return 1; if (!umi_ai_dev_repository_index_state_ready(&v)) return 2; if (umi_ai_dev_repository_index_state_health_score(&v) != 100U) return 3; return 0; }
