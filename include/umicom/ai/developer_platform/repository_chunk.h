/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/repository_chunk.h
 *
 * PURPOSE:
 *   Describe a bounded code/text chunk used for retrieval.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_REPOSITORY_CHUNK_H
#define UMICOM_AI_DEVELOPER_PLATFORM_REPOSITORY_CHUNK_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevRepositoryChunk {
    char id[UMI_AI_DEV_ID_CAPACITY];
    char label[UMI_AI_DEV_TEXT_CAPACITY];
    uint64_t revision;
    uint64_t flags;
    uint32_t priority;
    int enabled;
} UmiAiDevRepositoryChunk;

void umi_ai_dev_repository_chunk_init(UmiAiDevRepositoryChunk *value);
UmiStatus umi_ai_dev_repository_chunk_configure(UmiAiDevRepositoryChunk *value, const char *id, const char *label, uint32_t priority, uint64_t flags);
UmiStatus umi_ai_dev_repository_chunk_validate(const UmiAiDevRepositoryChunk *value);
uint32_t umi_ai_dev_repository_chunk_evidence_score(const UmiAiDevRepositoryChunk *value, uint32_t relevance);

#ifdef __cplusplus
}
#endif

#endif
