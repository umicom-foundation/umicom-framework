/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_knowledge_types.c
 * PURPOSE: Verify stable source kinds, hashes and metadata filters.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/knowledge/types.h"

int main(void)
{
    UmiKnowledgeChunk chunk = {0};
    UmiKnowledgeFilter filter = {0};
    assert(umi_knowledge_hash_text("Umicom") != 0U);
    assert(umi_knowledge_hash_text("Umicom") ==
           umi_knowledge_hash_text("Umicom"));
    assert(strcmp(umi_knowledge_source_kind_text(
               UMI_KNOWLEDGE_SOURCE_CODE), "code") == 0);
    (void)strcpy(chunk.collection_id, "framework");
    (void)strcpy(chunk.language, "c");
    chunk.kind = UMI_KNOWLEDGE_SOURCE_CODE;
    chunk.classification = UMI_AI_DATA_INTERNAL;
    (void)strcpy(filter.collection_id, "framework");
    filter.source_kind = UMI_KNOWLEDGE_SOURCE_CODE;
    filter.maximum_classification = UMI_AI_DATA_CONFIDENTIAL;
    assert(umi_knowledge_filter_matches(&filter, &chunk));
    (void)strcpy(filter.language, "python");
    assert(!umi_knowledge_filter_matches(&filter, &chunk));
    return 0;
}
