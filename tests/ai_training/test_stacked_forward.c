/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_forward.c
 *
 * PURPOSE:
 *   Check repeatable multi-layer decoder forward results and depth effects.
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
    UmiAiTrainingStackedDecoder *first = NULL, *repeat = NULL, *deep = NULL;
    size_t inputs[4] = {1U, 5U, 6U, 7U};
    float firstLogits[4U * 16U], repeatLogits[4U * 16U], deepLogits[4U * 16U];
    CHECK(UmiAiTrainingStackedDecoderCreate(&cfg, &first) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderCreate(&cfg, &repeat) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderLogits(first, inputs, 4U,
          firstLogits, 4U * 16U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderLogits(repeat, inputs, 4U,
          repeatLogits, 4U * 16U) == UMI_STATUS_OK);
    CHECK(memcmp(firstLogits, repeatLogits, sizeof(firstLogits)) == 0);
    cfg.transformerLayers = 3U;
    CHECK(UmiAiTrainingStackedDecoderCreate(&cfg, &deep) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderLogits(deep, inputs, 4U,
          deepLogits, 4U * 16U) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 4U * 16U; ++i) CHECK(isfinite(deepLogits[i]));
    UmiAiTrainingStackedDecoderDestroy(deep);
    UmiAiTrainingStackedDecoderDestroy(repeat);
    UmiAiTrainingStackedDecoderDestroy(first);
    puts("stacked model deterministic forward across 2 and 3 layers passed");
    return 0;
}
