/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/knowledge/catalogue.h
 * PURPOSE: Track indexed sources and their incremental-refresh identities.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* The catalogue stores source identity; the vector index stores chunks. */
#ifndef UMICOM_KNOWLEDGE_CATALOGUE_H
#define UMICOM_KNOWLEDGE_CATALOGUE_H

#include "umicom/knowledge/source.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the knowledge catalogue data shared with callers of this public contract.
 */
typedef struct UmiKnowledgeCatalogue UmiKnowledgeCatalogue;

/**
 * Initialise knowledge catalogue from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_knowledge_catalogue_create(size_t capacity,
                                         UmiKnowledgeCatalogue **out_catalogue);
/**
 * Release or reset state held by knowledge catalogue so the same storage can be reused
 * safely.
 */
void umi_knowledge_catalogue_destroy(UmiKnowledgeCatalogue *catalogue);
/**
 * Provide the knowledge catalogue upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_knowledge_catalogue_upsert(UmiKnowledgeCatalogue *catalogue,
                                         const UmiKnowledgeSource *source);
/**
 * Find knowledge catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_knowledge_catalogue_find(
    const UmiKnowledgeCatalogue *catalogue,
    const char *source_id,
    UmiKnowledgeSource *out_source);
/**
 * Remove knowledge catalogue while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_knowledge_catalogue_remove(UmiKnowledgeCatalogue *catalogue,
                                         const char *source_id);
/**
 * Return the number of records represented by knowledge catalogue without changing their
 * state.
 */
size_t umi_knowledge_catalogue_count(
    const UmiKnowledgeCatalogue *catalogue);
/**
 * Find knowledge catalogue source while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_knowledge_catalogue_source_at(
    const UmiKnowledgeCatalogue *catalogue,
    size_t position,
    UmiKnowledgeSource *out_source);

#ifdef __cplusplus
}
#endif
#endif
