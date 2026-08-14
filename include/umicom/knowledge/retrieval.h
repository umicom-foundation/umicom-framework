/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/knowledge/retrieval.h
 * PURPOSE: Execute filtered retrieval and attach exact source provenance.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Every returned match carries a URI and exact byte/line range. */
#ifndef UMICOM_KNOWLEDGE_RETRIEVAL_H
#define UMICOM_KNOWLEDGE_RETRIEVAL_H

#include "umicom/knowledge/catalogue.h"
#include "umicom/knowledge/embedding_provider.h"
#include "umicom/knowledge/vector_index.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiKnowledgeQuery {
    char text[UMI_KNOWLEDGE_TEXT_CAPACITY];
    UmiKnowledgeFilter filter;
    size_t limit;
    double minimum_score;
} UmiKnowledgeQuery;

UmiStatus umi_knowledge_query_init(UmiKnowledgeQuery *query,
                                   const char *text);
UmiStatus umi_knowledge_retrieve(
    const UmiKnowledgeCatalogue *catalogue,
    const UmiKnowledgeVectorIndex *index,
    const UmiKnowledgeEmbeddingProviderV1 *provider,
    const UmiKnowledgeQuery *query,
    UmiKnowledgeMatch *matches,
    size_t capacity,
    size_t *out_count);

#ifdef __cplusplus
}
#endif
#endif
