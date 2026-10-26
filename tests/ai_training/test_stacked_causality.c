/* Umicom Framework | causal invariance across Transformer layers | Sammy Hegab | MIT */
#include "stacked_test_fixture.h"
int main(void)
{
    UmiAiTrainingStackedDecoderConfig cfg = TestStackedConfig();
    UmiAiTrainingStackedDecoder *model = NULL;
    size_t a[4] = {1U, 5U, 6U, 7U}, b[4] = {1U, 5U, 9U, 10U};
    float x[4U * 16U], y[4U * 16U];
    cfg.transformerLayers = 3U;
    CHECK(UmiAiTrainingStackedDecoderCreate(&cfg, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderLogits(model, a, 4U, x, 64U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderLogits(model, b, 4U, y, 64U) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 32U; ++i)
        CHECK(fabsf(x[i] - y[i]) < 0.00001F);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("three-layer causal future-token exclusion passed");
    return 0;
}
