/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/knowledge/ingestion.h
 * PURPOSE: Coordinate incremental chunking, embedding and index replacement.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: Ingestion chunks and embeds first, then replaces changed source entries. */
#ifndef UMICOM_KNOWLEDGE_INGESTION_H
#define UMICOM_KNOWLEDGE_INGESTION_H

#include "umicom/knowledge/catalogue.h"
#include "umicom/knowledge/chunker.h"
#include "umicom/knowledge/embedding_provider.h"
#include "umicom/knowledge/vector_index.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiKnowledgeIngestionReport {
    UmiKnowledgeRefreshDecision decision;
    size_t chunks_created;
    size_t chunks_replaced;
    uint64_t content_hash;
    uint64_t index_revision;
} UmiKnowledgeIngestionReport;

UmiStatus umi_knowledge_ingest_text(
    UmiKnowledgeCatalogue *catalogue,
    UmiKnowledgeVectorIndex *index,
    const UmiKnowledgeEmbeddingProvider *provider,
    const UmiKnowledgeChunkPolicy *policy,
    const UmiKnowledgeSource *source,
    const char *text,
    UmiKnowledgeIngestionReport *out_report);

#ifdef __cplusplus
}
#endif
#endif
