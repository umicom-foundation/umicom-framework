/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_retrieval.c
 *
 * PURPOSE:
 *   Validate the AI and Helix foundation introduced by Batch 10.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused executable keeps one contract easy to diagnose when the larger test suite reports a failure.
 */



#include <assert.h>
#include <string.h>
#include "umicom/umicom.h"

int main(void)
{
    UmiAiChunk chunks[8];
    UmiAiRetrievalResult results[4];
    size_t count = umi_ai_chunk_text("doc",
        "Umicom Framework provides reusable services. Studio uses Umicom Framework.",
        32U, chunks, 8U);
    size_t found;
    assert(count >= 2U);
    found = umi_ai_retrieval_rank("Framework", chunks, count, results, 4U);
    assert(found > 0U);
    assert(strstr(results[0].chunk.text, "Framework") != NULL);
    return 0;
}
