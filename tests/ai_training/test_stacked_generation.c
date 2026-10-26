/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_generation.c
 *
 * PURPOSE:
 *   Check greedy token generation using the complete retained context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "stacked_test_fixture.h"
int main(void)
{
    UmiAiTrainingStackedDecoderConfig cfg = TestStackedConfig();
    UmiAiTrainingStackedDecoder *model = NULL;
    const size_t context[4] = {1U, 5U, 6U, 7U};
    size_t predicted = 999U, generated[8] = {0}, count = 0U;
    CHECK(UmiAiTrainingStackedDecoderCreate(&cfg, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderPredictNext(model, context, 4U,
          &predicted) == UMI_STATUS_OK);
    CHECK(predicted < (size_t)cfg.vocabularySize);
    CHECK(UmiAiTrainingStackedDecoderGenerateGreedy(model, context, 4U,
          8U, generated, 8U, &count) == UMI_STATUS_OK);
    CHECK(count == 8U && generated[0] == predicted);
    for (size_t i = 0U; i < 8U; ++i)
        CHECK(generated[i] < (size_t)cfg.vocabularySize);
    size_t sentinel = 8123U;
    CHECK(UmiAiTrainingStackedDecoderGenerateGreedy(model, context, 4U,
          2U, &sentinel, 1U, &count) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(sentinel == 8123U && count == 8U);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("stacked model greedy generation, raw IDs and capacity checks passed");
    return 0;
}
