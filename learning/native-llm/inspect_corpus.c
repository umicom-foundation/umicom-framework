/*-----------------------------------------------------------------------------
 * Umicom Framework | learning/native-llm/inspect_corpus.c
 * PURPOSE: Read-only inspection of a local corpus and next-token examples.
 * The Framework supplies all parsing and tokenisation. Never modifies source.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------
 * File: learning/native-llm/inspect_corpus.c
 */
#include "umicom/ai/training/corpus.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include <inttypes.h>
#include <stdio.h>
int main(int argc, char **argv)
{
    UmiAiCorpus *corpus = NULL;
    size_t length = 0U;
    uint32_t input[8], target[8];
    UmiStatus status;
    if (argc != 2 || argv[1][0] == '\0') {
        fprintf(stderr, "Usage: umicom-native-llm-corpus <approved-local-file>\n");
        return 2;
    }
    status = UmiAiCorpusLoadFile(argv[1], &corpus);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Corpus rejected (status %d; max %u bytes).\n",
                (int)status, (unsigned)UMI_AI_CORPUS_MAX_BYTES);
        return 1;
    }
    (void)UmiAiCorpusLength(corpus, &length);
    printf("Loaded %zu bytes; byte vocabulary %u.\n", length,
           (unsigned)UMI_AI_BYTE_VOCABULARY);
    if (length >= 2U) {
        size_t window = length - 1U < 8U ? length - 1U : 8U;
        status = UmiAiCorpusWindow(corpus, 0U, window, input, target, 8U);
        if (status != UMI_STATUS_OK) { UmiAiCorpusDestroy(corpus); return 1; }
        for (size_t i = 0U; i < window; ++i) {
            printf("position %zu: input %" PRIu32 " -> target %" PRIu32 "\n",
                   i, input[i], target[i]);
        }
    }
    UmiAiCorpusDestroy(corpus);
    return 0;
}
