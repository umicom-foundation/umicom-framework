/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/knowledge/types.h
 *
 * PURPOSE:
 *   Define the stable C23 value types shared by native retrieval, knowledge
 *   ingestion, citation and offline-index services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * These structures describe data.  Model inference remains owned by
 * AuthorEngine; the Knowledge Centre only prepares traceable evidence.
 */
#ifndef UMICOM_KNOWLEDGE_TYPES_H
#define UMICOM_KNOWLEDGE_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/ai/privacy_policy.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_KNOWLEDGE_ID_CAPACITY 96U
#define UMI_KNOWLEDGE_NAME_CAPACITY 192U
#define UMI_KNOWLEDGE_URI_CAPACITY 768U
#define UMI_KNOWLEDGE_TEXT_CAPACITY 2048U
#define UMI_KNOWLEDGE_LANGUAGE_CAPACITY 32U
#define UMI_KNOWLEDGE_MEDIA_TYPE_CAPACITY 96U
#define UMI_KNOWLEDGE_EMBEDDING_MAX 256U
#define UMI_KNOWLEDGE_PROVIDER_MAX 8U
#define UMI_KNOWLEDGE_COLLECTION_MAX 32U
#define UMI_KNOWLEDGE_QUERY_RESULT_MAX 64U

typedef enum UmiKnowledgeSourceKind {
    UMI_KNOWLEDGE_SOURCE_DOCUMENT = 1,
    UMI_KNOWLEDGE_SOURCE_CODE = 2,
    UMI_KNOWLEDGE_SOURCE_PROJECT = 3,
    UMI_KNOWLEDGE_SOURCE_WEB_ARCHIVE = 4,
    UMI_KNOWLEDGE_SOURCE_GENERATED = 5
} UmiKnowledgeSourceKind;

typedef enum UmiKnowledgeDistanceMetric {
    UMI_KNOWLEDGE_DISTANCE_COSINE = 1,
    UMI_KNOWLEDGE_DISTANCE_DOT = 2
} UmiKnowledgeDistanceMetric;

typedef enum UmiKnowledgeRefreshDecision {
    UMI_KNOWLEDGE_REFRESH_NEW = 1,
    UMI_KNOWLEDGE_REFRESH_UNCHANGED = 2,
    UMI_KNOWLEDGE_REFRESH_REPLACE = 3
} UmiKnowledgeRefreshDecision;

typedef struct UmiKnowledgeSource {
    char source_id[UMI_KNOWLEDGE_ID_CAPACITY];
    char collection_id[UMI_KNOWLEDGE_ID_CAPACITY];
    char title[UMI_KNOWLEDGE_NAME_CAPACITY];
    char uri[UMI_KNOWLEDGE_URI_CAPACITY];
    char language[UMI_KNOWLEDGE_LANGUAGE_CAPACITY];
    char media_type[UMI_KNOWLEDGE_MEDIA_TYPE_CAPACITY];
    UmiKnowledgeSourceKind kind;
    UmiAiDataClassification classification;
    uint64_t revision;
    uint64_t content_hash;
    uint64_t modified_ns;
    uint64_t size_bytes;
    int active;
} UmiKnowledgeSource;

typedef struct UmiKnowledgeChunk {
    char chunk_id[UMI_KNOWLEDGE_ID_CAPACITY];
    char source_id[UMI_KNOWLEDGE_ID_CAPACITY];
    char collection_id[UMI_KNOWLEDGE_ID_CAPACITY];
    char language[UMI_KNOWLEDGE_LANGUAGE_CAPACITY];
    char media_type[UMI_KNOWLEDGE_MEDIA_TYPE_CAPACITY];
    UmiKnowledgeSourceKind kind;
    UmiAiDataClassification classification;
    size_t ordinal;
    size_t byte_start;
    size_t byte_end;
    size_t line_start;
    size_t line_end;
    uint64_t source_revision;
    char text[UMI_KNOWLEDGE_TEXT_CAPACITY];
} UmiKnowledgeChunk;

typedef struct UmiKnowledgeEmbedding {
    float values[UMI_KNOWLEDGE_EMBEDDING_MAX];
    size_t dimension;
} UmiKnowledgeEmbedding;

typedef struct UmiKnowledgeFilter {
    char collection_id[UMI_KNOWLEDGE_ID_CAPACITY];
    char language[UMI_KNOWLEDGE_LANGUAGE_CAPACITY];
    char media_type[UMI_KNOWLEDGE_MEDIA_TYPE_CAPACITY];
    UmiKnowledgeSourceKind source_kind;
    UmiAiDataClassification maximum_classification;
} UmiKnowledgeFilter;

typedef struct UmiKnowledgeCitation {
    char source_id[UMI_KNOWLEDGE_ID_CAPACITY];
    char title[UMI_KNOWLEDGE_NAME_CAPACITY];
    char uri[UMI_KNOWLEDGE_URI_CAPACITY];
    size_t byte_start;
    size_t byte_end;
    size_t line_start;
    size_t line_end;
    uint64_t source_revision;
} UmiKnowledgeCitation;

typedef struct UmiKnowledgeMatch {
    UmiKnowledgeChunk chunk;
    UmiKnowledgeCitation citation;
    double score;
} UmiKnowledgeMatch;

uint64_t umi_knowledge_hash_text(const char *text);
const char *umi_knowledge_source_kind_text(UmiKnowledgeSourceKind kind);
int umi_knowledge_filter_matches(const UmiKnowledgeFilter *filter,
                                 const UmiKnowledgeChunk *chunk);

#ifdef __cplusplus
}
#endif

#endif
