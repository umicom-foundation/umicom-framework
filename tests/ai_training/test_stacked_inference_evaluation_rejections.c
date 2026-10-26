/* Umicom Framework | Held-out evaluation rejection tests | Sammy Hegab | MIT */
#include "stacked_inference_fixture.h"
int main(void)
{
    UmiAiTrainingStackedDecoder *model = NULL;
    const size_t inputs[] = {1U, 109U, 114U, 120U};
    size_t targets[] = {0U, 0U, 0U, 89U};
    uint8_t mask[] = {0U, 0U, 0U, 1U};
    UmiAiTrainingStackedEvaluationCase item = Example(inputs, targets, mask);
    UmiAiTrainingStackedEvaluationReport result;
    memset(&result, 0x75, sizeof(result));
    UmiAiTrainingStackedEvaluationReport unchanged = result;
    CHECK(MakeModel(UINT64_C(4), &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedEvaluationRun(model, &item, 0U,
        &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&result, &unchanged, sizeof(result)) == 0);
    mask[3] = 2U;
    CHECK(UmiAiTrainingStackedEvaluationRun(model, &item, 1U,
        &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&result, &unchanged, sizeof(result)) == 0);
    mask[3] = 0U;
    CHECK(UmiAiTrainingStackedEvaluationRun(model, &item, 1U,
        &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&result, &unchanged, sizeof(result)) == 0);
    mask[3] = 1U;
    targets[3] = 999U;
    CHECK(UmiAiTrainingStackedEvaluationRun(model, &item, 1U,
        &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&result, &unchanged, sizeof(result)) == 0);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("stacked held-out evaluator: malformed windows rejected atomically");
    return 0;
}
