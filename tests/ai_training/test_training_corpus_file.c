/* Umicom Framework | Sammy Hegab / Umicom Foundation | MIT
 * Exercise native corpus file ingestion with temporary local-only fixtures.
 * File: tests/ai_training/test_training_corpus_file.c
 * PURPOSE: Exercise native corpus ingestion using temporary local files.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 */
#include "umicom/ai/training/corpus.h"
#include <stdio.h>
#define CHECK(p) do { if (!(p)) { fprintf(stderr, "FAIL line %d\n", __LINE__); return 1; } } while (0)
int main(void)
{
    const char *path = "umicom_batch05_corpus_fixture.bin";
    const unsigned char bytes[] = { 0U, 255U, 'C', '\n' };
    FILE *file = fopen(path, "wb");
    UmiAiCorpus *corpus = NULL;
    size_t length = 0U;
    uint32_t x[2] = { 0U, 0U }, y[2] = { 0U, 0U };
    CHECK(file != NULL);
    CHECK(fwrite(bytes, 1U, sizeof(bytes), file) == sizeof(bytes));
    CHECK(fclose(file) == 0);
    CHECK(UmiAiCorpusLoadFile(path, &corpus) == UMI_STATUS_OK);
    CHECK(UmiAiCorpusLength(corpus, &length) == UMI_STATUS_OK && length == sizeof(bytes));
    CHECK(UmiAiCorpusWindow(corpus, 1U, 2U, x, y, 2U) == UMI_STATUS_OK);
    CHECK(x[0] == 259U && y[0] == (uint32_t)'C' + 4U);
    CHECK(x[1] == (uint32_t)'C' + 4U && y[1] == (uint32_t)'\n' + 4U);
    CHECK(remove(path) == 0);
    UmiAiCorpusDestroy(corpus);
    corpus = NULL;
    CHECK(UmiAiCorpusLoadFile(path, &corpus) == UMI_STATUS_IO_ERROR);
    CHECK(corpus == NULL);
    puts("PASS: local corpus file load and unavailable path");
    return 0;
}
