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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the ai dev repository index queue data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDevRepositoryIndexQueue { uint64_t items[UMI_AI_DEV_SMALL_CAPACITY]; size_t head; size_t count; } UmiAiDevRepositoryIndexQueue;
/**
 * Initialise ai dev repository index queue from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_repository_index_queue_init(UmiAiDevRepositoryIndexQueue *queue);
/**
 * Provide the ai dev repository index queue push operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_dev_repository_index_queue_push(UmiAiDevRepositoryIndexQueue *queue,uint64_t item);
/**
 * Provide the ai dev repository index queue pop operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_dev_repository_index_queue_pop(UmiAiDevRepositoryIndexQueue *queue,uint64_t *out_item);
/**
 * Return the number of records represented by ai dev repository index queue without
 * changing their state.
 */
size_t umi_ai_dev_repository_index_queue_count(const UmiAiDevRepositoryIndexQueue *queue);

#ifdef __cplusplus
}
#endif

#endif
