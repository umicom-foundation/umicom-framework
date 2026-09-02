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

/**
 * Represent the ai dev repository corpus data shared with callers of this public contract.
 */
typedef struct UmiAiDevRepositoryCorpus { char ids[UMI_AI_DEV_MEDIUM_CAPACITY][UMI_AI_DEV_ID_CAPACITY]; size_t count; uint64_t revision; } UmiAiDevRepositoryCorpus;
/**
 * Initialise ai dev repository corpus from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_repository_corpus_init(UmiAiDevRepositoryCorpus *collection);
/**
 * Add ai dev repository corpus only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ai_dev_repository_corpus_add(UmiAiDevRepositoryCorpus *collection, const char *id);
/**
 * Remove ai dev repository corpus while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ai_dev_repository_corpus_remove(UmiAiDevRepositoryCorpus *collection, const char *id);
/**
 * Provide the ai dev repository corpus contains operation used by this module and its
 * client applications.
 */
int umi_ai_dev_repository_corpus_contains(const UmiAiDevRepositoryCorpus *collection, const char *id);
/**
 * Return the number of records represented by ai dev repository corpus without changing
 * their state.
 */
size_t umi_ai_dev_repository_corpus_count(const UmiAiDevRepositoryCorpus *collection);

#ifdef __cplusplus
}
#endif

#endif
