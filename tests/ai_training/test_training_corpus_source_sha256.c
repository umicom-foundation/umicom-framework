/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_corpus_source_sha256.c
 *
 * PURPOSE:
 *   Confirm that immutable corpus SHA-256 hashes original bytes rather than
 *   token IDs or a re-read file, and preserves outputs on invalid arguments.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/corpus.h"
#include "umicom/base/sha256.h"
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

int main(void)
{
    uint8_t input[3] = {'a', 'b', 'c'};
    unsigned char digest[UMI_AI_CORPUS_SHA256_BYTES];
    unsigned char second[UMI_AI_CORPUS_SHA256_BYTES];
    char hex[UMI_SHA256_HEX_CAPACITY];
    UmiAiCorpus *corpus = NULL;

    CHECK(UmiAiCorpusCreate(input, sizeof(input), &corpus) == UMI_STATUS_OK);
    CHECK(UmiAiCorpusSha256(corpus, digest) == UMI_STATUS_OK);
    UmiSha256Hex(digest, hex);
    CHECK(strcmp(hex,
        "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad") == 0);

    /* The caller's original byte array is copied by CorpusCreate. */
    input[0] = 'x';
    CHECK(UmiAiCorpusSha256(corpus, second) == UMI_STATUS_OK);
    CHECK(memcmp(digest, second, sizeof(digest)) == 0);

    memset(second, 0xA5, sizeof(second));
    CHECK(UmiAiCorpusSha256(NULL, second) == UMI_STATUS_INVALID_ARGUMENT);
    for (size_t i = 0U; i < sizeof(second); ++i) CHECK(second[i] == 0xA5U);
    CHECK(UmiAiCorpusSha256(corpus, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    UmiAiCorpusDestroy(corpus);
    puts("corpus source SHA-256: immutable bytes, known vector, atomic rejection PASS");
    return 0;
}
