/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_corpus_ranges.c
 *
 * PURPOSE:
 *   Check token range aliasing, boundary arithmetic and unchanged outputs on refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/corpus.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)

int main(void)
{
    const uint8_t source[] = {'a', 'b', 'c', 'd', 'e', 'f'};
    UmiAiCorpus *corpus = NULL;
    uint32_t slots[12];
    uint32_t before[12];
    CHECK(UmiAiCorpusCreate(source, sizeof(source), &corpus) == UMI_STATUS_OK);
    /* Use real arrays in both directions. Synthetic addresses would test a
     * pointer representation rather than a valid caller-owned allocation. */
    for (size_t displacement = 0U; displacement < 4U; ++displacement) {
        for (size_t i = 0U; i < 12U; ++i) slots[i] = (uint32_t)(900U + i);
        memcpy(before, slots, sizeof(slots));
        CHECK(UmiAiCorpusWindow(corpus, 0U, 4U, slots + 1U,
                               slots + 1U + displacement, 4U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before, slots, sizeof(slots)) == 0);
        CHECK(UmiAiCorpusWindow(corpus, 0U, 4U, slots + 1U + displacement,
                               slots + 1U, 4U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before, slots, sizeof(slots)) == 0);
    }
    /* Half-open ranges [1,5) and [5,9) touch without sharing an element. */
    CHECK(UmiAiCorpusWindow(corpus, 1U, 4U, slots + 1U, slots + 5U, 4U) == UMI_STATUS_OK);
    CHECK(slots[0] == before[0] && slots[9] == before[9]);
    for (size_t i = 0U; i < 4U; ++i) {
        CHECK(slots[1U + i] == (uint32_t)source[1U + i] + UMI_AI_BYTE_OFFSET);
        CHECK(slots[5U + i] == (uint32_t)source[2U + i] + UMI_AI_BYTE_OFFSET);
    }
    CHECK(UmiAiCorpusWindow(corpus, 0U, 4U, slots + 5U, slots + 1U, 4U) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 4U; ++i) {
        CHECK(slots[5U + i] == (uint32_t)source[i] + UMI_AI_BYTE_OFFSET);
        CHECK(slots[1U + i] == (uint32_t)source[i + 1U] + UMI_AI_BYTE_OFFSET);
    }
    memcpy(before, slots, sizeof(slots));
    CHECK(UmiAiCorpusWindow(corpus, 0U, 4U, slots + 1U, slots + 5U, 3U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(before, slots, sizeof(slots)) == 0);
    CHECK(UmiAiCorpusWindow(corpus, SIZE_MAX, 1U, slots, slots + 5U, 4U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiCorpusWindow(corpus, 0U, SIZE_MAX, slots, slots + 5U, 4U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiCorpusWindow(corpus, 5U, 1U, slots, slots + 5U, 4U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(before, slots, sizeof(slots)) == 0);
    CHECK(UmiAiCorpusWindow(corpus, 4U, 1U, slots, slots + 1U, 1U) == UMI_STATUS_OK);
    CHECK(slots[0] == (uint32_t)'e' + UMI_AI_BYTE_OFFSET);
    CHECK(slots[1] == (uint32_t)'f' + UMI_AI_BYTE_OFFSET);
    UmiAiCorpusDestroy(corpus);
    puts("Corpus output ranges: passed");
    return 0;
}
