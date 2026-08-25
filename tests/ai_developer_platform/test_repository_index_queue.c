#include "umicom/ai/developer_platform/repository_index_queue.h"
int main(void) { UmiAiDevRepositoryIndexQueue q; uint64_t v=0U; umi_ai_dev_repository_index_queue_init(&q); if(umi_ai_dev_repository_index_queue_push(&q,9U)!=UMI_STATUS_OK)return 1; if(umi_ai_dev_repository_index_queue_pop(&q,&v)!=UMI_STATUS_OK||v!=9U)return 2; return 0; }
