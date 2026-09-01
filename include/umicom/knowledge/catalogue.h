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

typedef struct UmiKnowledgeCatalogue UmiKnowledgeCatalogue;

UmiStatus umi_knowledge_catalogue_create(size_t capacity,
                                         UmiKnowledgeCatalogue **out_catalogue);
void umi_knowledge_catalogue_destroy(UmiKnowledgeCatalogue *catalogue);
UmiStatus umi_knowledge_catalogue_upsert(UmiKnowledgeCatalogue *catalogue,
                                         const UmiKnowledgeSource *source);
UmiStatus umi_knowledge_catalogue_find(
    const UmiKnowledgeCatalogue *catalogue,
    const char *source_id,
    UmiKnowledgeSource *out_source);
UmiStatus umi_knowledge_catalogue_remove(UmiKnowledgeCatalogue *catalogue,
                                         const char *source_id);
size_t umi_knowledge_catalogue_count(
    const UmiKnowledgeCatalogue *catalogue);
UmiStatus umi_knowledge_catalogue_source_at(
    const UmiKnowledgeCatalogue *catalogue,
    size_t position,
    UmiKnowledgeSource *out_source);

#ifdef __cplusplus
}
#endif
#endif
