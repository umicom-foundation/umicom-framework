/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/repository_index_queue.h
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_REPOSITORY_INDEX_QUEUE_H
#define UMICOM_AI_DEVELOPER_PLATFORM_REPOSITORY_INDEX_QUEUE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevRepositoryIndexQueue { uint64_t items[UMI_AI_DEV_SMALL_CAPACITY]; size_t head; size_t count; } UmiAiDevRepositoryIndexQueue;
void umi_ai_dev_repository_index_queue_init(UmiAiDevRepositoryIndexQueue *queue);
UmiStatus umi_ai_dev_repository_index_queue_push(UmiAiDevRepositoryIndexQueue *queue,uint64_t item);
UmiStatus umi_ai_dev_repository_index_queue_pop(UmiAiDevRepositoryIndexQueue *queue,uint64_t *out_item);
size_t umi_ai_dev_repository_index_queue_count(const UmiAiDevRepositoryIndexQueue *queue);

#ifdef __cplusplus
}
#endif

#endif
