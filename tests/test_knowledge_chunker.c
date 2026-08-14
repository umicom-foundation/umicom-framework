/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_knowledge_chunker.c
 * PURPOSE: Verify overlap and exact source ranges survive deterministic chunking.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/knowledge/chunker.h"

int main(void)
{
    UmiKnowledgeSource source;
    UmiKnowledgeChunkPolicy policy = {12U, 4U, 4U, 1};
    UmiKnowledgeChunk chunks[8];
    size_t count = 0U;
    assert(umi_knowledge_source_init(
        &source, "doc.one", "manuals", "Manual", "file://manual.txt",
        UMI_KNOWLEDGE_SOURCE_DOCUMENT) == UMI_STATUS_OK);
    assert(umi_knowledge_chunk_text(
        &source, "line one\nline two\nline three\n", &policy,
        chunks, 8U, &count) == UMI_STATUS_OK);
    assert(count >= 3U);
    assert(chunks[0].byte_start == 0U);
    assert(chunks[1].byte_start < chunks[0].byte_end);
    assert(chunks[1].line_start >= 1U);
    assert(strcmp(chunks[0].source_id, "doc.one") == 0);
    return 0;
}
