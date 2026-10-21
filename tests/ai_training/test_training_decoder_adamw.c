/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_decoder_adamw.c
 * PURPOSE: Confirm seeded decoder training and AdamW binding/step integrity.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/optimizer.h"
#include "umicom/ai/training/checkpoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

int main(void)
{
    UmiAiTrainingDecoderConfig config = {12U, 3U, 4U, 8U, 2U, 341U};
    UmiAiTrainingDecoder *a = NULL, *b = NULL, *loaded = NULL;
    UmiAiTrainingAdamW *first = NULL, *second = NULL, *refused = NULL;
    UmiAiTrainingAdamWConfig options;
    size_t inputs[] = {1U,2U,3U}, targets[] = {2U,3U,4U};
    float la = -1.0F, lb = -2.0F;
    size_t bytesA = 0U, bytesB = 0U, usedA = 0U, usedB = 0U;
    uint8_t *bufferA = NULL, *bufferB = NULL;
    uint64_t steps = 0U;
    size_t stateBytes = 0U;
    CHECK(UmiAiTrainingAdamWDefaults(&options) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&config, &a) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&config, &b) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderAdamWCreate(a, &options, &first) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderAdamWCreate(b, &options, &second) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderTrainAdamWStep(a, second, inputs, targets, 3U,
                                             0.02F, &la) == UMI_STATUS_INVALID_STATE);
    CHECK(la == -1.0F);
    for (size_t i = 0U; i < 8U; ++i) {
        CHECK(UmiAiTrainingDecoderTrainAdamWStep(a, first, inputs, targets,
                                                3U, 0.02F, &la) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingDecoderTrainAdamWStep(b, second, inputs, targets,
                                                3U, 0.02F, &lb) == UMI_STATUS_OK);
        CHECK(isfinite(la) && la == lb);
    }
    CHECK(UmiAiTrainingAdamWInspect(first, &steps, &stateBytes) == UMI_STATUS_OK);
    CHECK(steps == 8U && stateBytes > 0U);
    CHECK(UmiAiTrainingDecoderCheckpointSize(a, &bytesA) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointSize(b, &bytesB) == UMI_STATUS_OK);
    CHECK(bytesA == bytesB);
    bufferA = malloc(bytesA); bufferB = malloc(bytesB);
    CHECK(bufferA != NULL && bufferB != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(a, bufferA, bytesA, &usedA) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(b, bufferB, bytesB, &usedB) == UMI_STATUS_OK);
    CHECK(usedA == bytesA && usedB == bytesB && memcmp(bufferA, bufferB, bytesA) == 0);
    CHECK(UmiAiTrainingDecoderCheckpointDecode(bufferA, bytesA, &loaded) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderAdamWCreate(loaded, &options, &refused) == UMI_STATUS_INVALID_STATE);
    CHECK(refused == NULL);
    CHECK(UmiAiTrainingDecoderTrainStep(a, inputs, targets, 3U, 0.01F, &la) == UMI_STATUS_OK);
    la = -66.0F;
    CHECK(UmiAiTrainingDecoderTrainAdamWStep(a, first, inputs, targets, 3U,
                                            0.02F, &la) == UMI_STATUS_INVALID_STATE);
    CHECK(la == -66.0F);
    free(bufferB); free(bufferA);
    UmiAiTrainingAdamWDestroy(first); UmiAiTrainingAdamWDestroy(second);
    UmiAiTrainingDecoderDestroy(a); UmiAiTrainingDecoderDestroy(b);
    UmiAiTrainingDecoderDestroy(loaded);
    puts("seeded AdamW decoder training and checkpoint limitations: PASS");
    return 0;
}
