/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-llm/inspect_corpus_ranges.c
 *
 * PURPOSE:
 *   Demonstrate caller-owned token windows and unchanged output on overlap refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/corpus.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    const uint8_t source[] = {'a', 'b', 'c', 'd', 'e', 'f'};
    UmiAiCorpus *corpus = NULL;
    uint32_t storage[12];
    uint32_t before[12];
    UmiStatus status = UmiAiCorpusCreate(source, sizeof(source), &corpus);
    if (status != UMI_STATUS_OK) {
        fputs("Cannot create the example corpus.\n", stderr);
        return 1;
    }
    for (size_t i = 0U; i < 12U; ++i) storage[i] = 999U;
    memcpy(before, storage, sizeof(storage));
    /* Both pointers are inside real writable storage. Their four-element
     * windows intersect, so the Framework must refuse before publishing. */
    status = UmiAiCorpusWindow(corpus, 0U, 4U, storage + 1U, storage + 3U, 4U);
    if (status != UMI_STATUS_INVALID_ARGUMENT ||
        memcmp(before, storage, sizeof(storage)) != 0) {
        fputs("Overlapping outputs were not rejected without changes.\n", stderr);
        UmiAiCorpusDestroy(corpus);
        return 1;
    }
    puts("Overlapping outputs: rejected, storage unchanged");
    status = UmiAiCorpusWindow(corpus, 0U, 4U, storage + 1U, storage + 5U, 4U);
    if (status != UMI_STATUS_OK) {
        fputs("Adjacent output windows were rejected.\n", stderr);
        UmiAiCorpusDestroy(corpus);
        return 1;
    }
    puts("Adjacent outputs: accepted");
    for (size_t i = 0U; i < 4U; ++i)
        printf("pair %zu: input=%" PRIu32 " target=%" PRIu32 "\n",
               i, storage[1U + i], storage[5U + i]);
    UmiAiCorpusDestroy(corpus);
    return 0;
}
