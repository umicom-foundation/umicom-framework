/* Umicom Framework | Token-weighted held-out metrics | Sammy Hegab | MIT */
#include "stacked_inference_fixture.h"
int main(void)
{
    UmiAiTrainingStackedDecoder *model = NULL;
    const size_t inputs1[] = {1U, 109U, 114U, 120U};
    const size_t targets1[] = {0U, 0U, 0U, 89U};
    const uint8_t mask1[] = {0U, 0U, 0U, 1U};
    const size_t inputs2[] = {1U, 120U, 130U, 131U};
    const size_t targets2[] = {0U, 15U, 17U, 18U};
    const uint8_t mask2[] = {0U, 1U, 1U, 1U};
    UmiAiTrainingStackedEvaluationCase cases[2] = {
        Example(inputs1, targets1, mask1), Example(inputs2, targets2, mask2)
    };
    UmiAiTrainingStackedEvaluationReport result = {0}, repeated = {0};
    float loss1 = 0.0F, loss2 = 0.0F;
    CHECK(MakeModel(UINT64_C(8), &model) == UMI_STATUS_OK);
    cases[0].originalSourceSha256[0] = 11U;
    cases[1].originalSourceSha256[0] = 22U;
    CHECK(UmiAiTrainingStackedDecoderEvaluateSelected(model,
        inputs1, targets1, mask1, 4U, &loss1) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderEvaluateSelected(model,
        inputs2, targets2, mask2, 4U, &loss2) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedEvaluationRun(model, cases, 2U,
                                            &result) == UMI_STATUS_OK);
    CHECK(result.supervisedTargets == 4U && result.exampleCount == 2U);
    CHECK(fabs(result.meanSelectedCrossEntropy -
          ((double)loss1 + 3.0 * (double)loss2) / 4.0) < 0.000001);
    CHECK(result.correctTopOneTargets <= result.supervisedTargets);
    CHECK(result.topOneAccuracy >= 0.0 && result.topOneAccuracy <= 1.0);
    CHECK(result.perplexityAvailable == 1 && result.perplexity >= 1.0);
    CHECK(UmiAiTrainingStackedEvaluationRun(model, cases, 2U,
                                            &repeated) == UMI_STATUS_OK);
    CHECK(memcmp(result.orderedCaseSha256, repeated.orderedCaseSha256,
                 UMI_SHA256_BYTES) == 0);
    CHECK(result.meanSelectedCrossEntropy == repeated.meanSelectedCrossEntropy);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("stacked held-out evaluator: selected-token weighting verified");
    return 0;
}
