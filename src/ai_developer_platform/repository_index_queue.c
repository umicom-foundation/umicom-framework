/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/repository_index_queue.c
 *
 * PURPOSE:
 *   Queue bounded incremental repository index work.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/repository_index_queue.h"
#include <string.h>
void umi_ai_dev_repository_index_queue_init(UmiAiDevRepositoryIndexQueue *queue) {
    if (queue != NULL) {
        memset(queue, 0, sizeof(*queue));
    }
}
UmiStatus umi_ai_dev_repository_index_queue_push(UmiAiDevRepositoryIndexQueue *queue,uint64_t item) { size_t slot; if(queue==NULL||item==0U)return UMI_STATUS_INVALID_ARGUMENT; if(queue->count>=UMI_AI_DEV_SMALL_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED; slot=(queue->head+queue->count)%UMI_AI_DEV_SMALL_CAPACITY; queue->items[slot]=item; ++queue->count; return UMI_STATUS_OK; }
UmiStatus umi_ai_dev_repository_index_queue_pop(UmiAiDevRepositoryIndexQueue *queue,uint64_t *out_item) { if(queue==NULL||out_item==NULL)return UMI_STATUS_INVALID_ARGUMENT; if(queue->count==0U)return UMI_STATUS_NOT_FOUND; *out_item=queue->items[queue->head]; queue->head=(queue->head+1U)%UMI_AI_DEV_SMALL_CAPACITY; --queue->count; return UMI_STATUS_OK; }
size_t umi_ai_dev_repository_index_queue_count(const UmiAiDevRepositoryIndexQueue *queue) { return queue==NULL?0U:queue->count; }
