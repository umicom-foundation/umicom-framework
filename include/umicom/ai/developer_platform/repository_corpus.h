/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/developer_platform/repository_corpus.h
 *
 * PURPOSE:
 *   Track the bounded repository corpus participating in AI indexing.
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
#ifndef UMICOM_AI_DEVELOPER_PLATFORM_REPOSITORY_CORPUS_H
#define UMICOM_AI_DEVELOPER_PLATFORM_REPOSITORY_CORPUS_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/ai/developer_platform/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDevRepositoryCorpus { char ids[UMI_AI_DEV_MEDIUM_CAPACITY][UMI_AI_DEV_ID_CAPACITY]; size_t count; uint64_t revision; } UmiAiDevRepositoryCorpus;
void umi_ai_dev_repository_corpus_init(UmiAiDevRepositoryCorpus *collection);
UmiStatus umi_ai_dev_repository_corpus_add(UmiAiDevRepositoryCorpus *collection, const char *id);
UmiStatus umi_ai_dev_repository_corpus_remove(UmiAiDevRepositoryCorpus *collection, const char *id);
int umi_ai_dev_repository_corpus_contains(const UmiAiDevRepositoryCorpus *collection, const char *id);
size_t umi_ai_dev_repository_corpus_count(const UmiAiDevRepositoryCorpus *collection);

#ifdef __cplusplus
}
#endif

#endif
