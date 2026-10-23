/* Umicom Framework | Sammy Hegab / Umicom Foundation | MIT
 * Validate corpus copying, window alignment, and rejection without writes.
 * File: tests/ai_training/test_training_corpus.c
 * PURPOSE: Validate corpus copying, next-token window alignment and rejection without output writes.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 */
#include "umicom/ai/training/corpus.h"
#include <stdio.h>
#define CHECK(p) do { if (!(p)) { fprintf(stderr, "FAIL line %d\n", __LINE__); return 1; } } while (0)
int main(void)
{
    uint8_t data[] = { 'C', '2', '3', '\0', '!', '\n' };
    UmiAiCorpus *corpus = NULL;
    uint32_t input[4] = { 111U, 111U, 111U, 111U };
    uint32_t target[4] = { 222U, 222U, 222U, 222U };
    size_t size = 0U;
    CHECK(UmiAiCorpusCreate(data, sizeof(data), &corpus) == UMI_STATUS_OK);
    data[0] = 'X';
    CHECK(UmiAiCorpusLength(corpus, &size) == UMI_STATUS_OK && size == 6U);
    CHECK(UmiAiCorpusWindow(corpus, 0U, 4U, input, target, 4U) == UMI_STATUS_OK);
    CHECK(input[0] == (uint32_t)'C' + 4U && target[0] == (uint32_t)'2' + 4U);
    CHECK(input[3] == 4U && target[3] == (uint32_t)'!' + 4U);
    input[0] = 111U; target[0] = 222U;
    CHECK(UmiAiCorpusWindow(corpus, 4U, 2U, input, target, 4U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(input[0] == 111U && target[0] == 222U);
    CHECK(UmiAiCorpusWindow(corpus, 0U, 4U, input, target, 3U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(input[0] == 111U && target[0] == 222U);
    CHECK(UmiAiCorpusCreate(data, 0U, &corpus) == UMI_STATUS_INVALID_ARGUMENT);
    UmiAiCorpusDestroy(corpus);
    puts("PASS: corpus snapshots, causal targets, and bounds");
    return 0;
}
