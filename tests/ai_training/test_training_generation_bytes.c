/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_generation_bytes.c
 * PURPOSE: Reversible byte generation, embedded NUL and invalid byte outputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_generation_bytes.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/generation.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); \
    return 1; } } while (0)

int main(void)
{
    UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(455)};
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingSamplingOptions options = {0.7F, 5U};
    const uint8_t binaryPrompt[] = {0U, 42U, 0xE2U, 0x98U, 0x83U};
    uint32_t encoded[5];
    size_t prompt[5], generated[16];
    uint8_t decoded[16], untouched[16];
    size_t count = 123U, countIds = 0U;
    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiByteEncode(binaryPrompt, sizeof(binaryPrompt), encoded,
            5U, &countIds) == UMI_STATUS_OK && countIds == 5U);
    for (size_t i = 0U; i < 5U; ++i) prompt[i] = (size_t)encoded[i];
    CHECK(UmiAiTrainingDecoderGenerate(model, prompt, 5U,
            &options, 16U, 9902U, generated, 16U, &countIds) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderGenerateBytes(model, binaryPrompt,
            sizeof(binaryPrompt), &options, 16U, 9902U,
            decoded, 16U, &count) == UMI_STATUS_OK);
    CHECK(count == countIds && count == 16U);
    for (size_t i = 0U; i < count; ++i)
        CHECK((size_t)decoded[i] + UMI_AI_BYTE_OFFSET == generated[i]);
    memset(untouched, 0x5AU, sizeof(untouched));
    count = 999U;
    CHECK(UmiAiTrainingDecoderGenerateBytes(model, binaryPrompt,
            sizeof(binaryPrompt), &options, 16U, 9902U,
            untouched, 15U, &count) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(count == 999U);
    for (size_t i = 0U; i < sizeof(untouched); ++i) CHECK(untouched[i] == 0x5AU);
    CHECK(UmiAiTrainingDecoderGenerateBytes(model, NULL, 0U,
            &options, 4U, 12U, decoded, 16U, &count) == UMI_STATUS_OK);
    CHECK(count == 4U);
    /* Opt-in nucleus sampling uses exactly the same byte-token codec.
     * At topP=1 the original seeded output must remain unchanged. */
    UmiAiTrainingNucleusOptions pOne = {0.7F, 5U, 1.0F};
    UmiAiTrainingNucleusOptions nucleus = {0.7F, 5U, 0.85F};
    uint8_t legacyBytes[16], nucleusBytes[16], nucleusAgain[16];
    size_t expectedBytes = 0U, nucleusTokens = 0U;
    CHECK(UmiAiTrainingDecoderGenerateBytes(model, binaryPrompt,
        sizeof(binaryPrompt), &options, 16U, 9902U,
        legacyBytes, sizeof legacyBytes, &expectedBytes) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderGenerateBytesNucleus(model, binaryPrompt,
        sizeof(binaryPrompt), &pOne, 16U, 9902U,
        nucleusBytes, sizeof nucleusBytes, &count) == UMI_STATUS_OK);
    CHECK(count == expectedBytes && memcmp(legacyBytes, nucleusBytes, count) == 0);
    CHECK(UmiAiTrainingDecoderGenerateNucleus(model, prompt,
        sizeof prompt / sizeof prompt[0], &nucleus, 16U, 9902U,
        generated, sizeof generated / sizeof generated[0], &nucleusTokens) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderGenerateBytesNucleus(model, binaryPrompt,
        sizeof(binaryPrompt), &nucleus, 16U, 9902U,
        nucleusBytes, sizeof nucleusBytes, &count) == UMI_STATUS_OK);
    CHECK(count == nucleusTokens && count == 16U);
    for (size_t index = 0U; index < count; ++index)
        CHECK((size_t)nucleusBytes[index] + UMI_AI_BYTE_OFFSET == generated[index]);
    CHECK(UmiAiTrainingDecoderGenerateBytesNucleus(model, binaryPrompt,
        sizeof(binaryPrompt), &nucleus, 16U, 9902U,
        nucleusAgain, sizeof nucleusAgain, &count) == UMI_STATUS_OK);
    CHECK(memcmp(nucleusBytes, nucleusAgain, count) == 0);
    memset(untouched, 0x5AU, sizeof untouched);
    count = 999U;
    nucleus.topP = 0.0F;
    CHECK(UmiAiTrainingDecoderGenerateBytesNucleus(model, binaryPrompt,
        sizeof(binaryPrompt), &nucleus, 16U, 9902U,
        untouched, sizeof untouched, &count) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(count == 999U);
    for (size_t i = 0U; i < sizeof untouched; ++i) CHECK(untouched[i] == 0x5AU);
    UmiAiTrainingDecoderDestroy(model);
    puts("lossless native byte adapter, bounded output and empty prompt: PASS");
    return 0;
}
