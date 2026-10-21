/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_generation_checkpoint.c
 * PURPOSE: Stable sampling through save/reload of independently owned weights.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/checkpoint.h"
#include "umicom/ai/training/generation.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); \
    return 1; } } while (0)

int main(void)
{
    UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(173)};
    UmiAiTrainingDecoder *model = NULL, *restored = NULL;
    UmiAiTrainingSamplingOptions options = {0.65F, 11U};
    size_t input[] = {9U, 8U, 7U, 6U, 5U, 4U, 12U, 13U};
    size_t targets[] = {8U, 7U, 6U, 5U, 4U, 12U, 13U, 14U};
    float loss = 0.0F;
    size_t outA[16], outB[16], countA = 0U, countB = 0U;
    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderTrainStep(model, input, targets,
                8U, 0.005F, &loss) == UMI_STATUS_OK);
    size_t size = 0U, actual = 0U;
    CHECK(UmiAiTrainingDecoderCheckpointSize(model, &size) == UMI_STATUS_OK);
    uint8_t *snapshot = malloc(size);
    CHECK(snapshot != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, snapshot,
                size, &actual) == UMI_STATUS_OK && actual == size);
    CHECK(UmiAiTrainingDecoderCheckpointDecode(snapshot,
                size, &restored) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderGenerate(model, input, 8U,
                &options, 16U, 123123U, outA, 16U, &countA) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderGenerate(restored, input, 8U,
                &options, 16U, 123123U, outB, 16U, &countB) == UMI_STATUS_OK);
    CHECK(countA == 16U && countB == 16U);
    CHECK(memcmp(outA, outB, sizeof(outA)) == 0);
    UmiAiTrainingDecoderConfig readConfig;
    uint64_t steps = 0U;
    CHECK(UmiAiTrainingDecoderInspect(restored, &readConfig, &steps) == UMI_STATUS_OK);
    CHECK(steps == 1U);
    free(snapshot);
    UmiAiTrainingDecoderDestroy(restored);
    UmiAiTrainingDecoderDestroy(model);
    puts("checkpoint reload preserves seeded sampled generation: PASS");
    return 0;
}
