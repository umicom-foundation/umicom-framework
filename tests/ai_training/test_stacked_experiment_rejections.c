/* Umicom Framework | source-group and invalid-plan rejection | Sammy Hegab | MIT */
#include "stacked_test_fixture.h"
int main(void)
{
    UmiAiTrainingStackedDecoderConfig cfg = TestStackedConfig();
    UmiAiTrainingStackedDecoder *model = NULL;
    const size_t inputs[2][4] = {{1U,5U,6U,7U}, {1U,6U,8U,9U}};
    const size_t targets[2][4] = {{0U,6U,7U,2U}, {0U,8U,9U,2U}};
    const uint8_t mask[4] = {0U,1U,1U,1U};
    UmiAiTrainingStackedExample items[2] = {0};
    UmiAiTrainingStackedExperimentPlan plan = {1U,0.02F,4U};
    UmiAiTrainingStackedExperimentReport report = {0}, saved;
    UmiAiTrainingStackedDecoderInfo info = {0};
    for (size_t i = 0U; i < 2U; ++i) {
        items[i].inputIds = inputs[i];
        items[i].targetIds = targets[i];
        items[i].lossMask = mask;
        items[i].tokenCount = 4U;
        items[i].partition = i == 0U
            ? UMI_AI_TRAINING_PARTITION_TRAIN : UMI_AI_TRAINING_PARTITION_EVALUATE;
        TestStackedSourceDigest(items[i].originalSourceSha256, (uint8_t)(7U + i));
    }
    CHECK(UmiAiTrainingStackedDecoderCreate(&cfg, &model) == UMI_STATUS_OK);
    memset(&report, 0xa5, sizeof(report));
    saved = report;
    plan.epochs = 0U;
    CHECK(UmiAiTrainingStackedExperimentRun(model, items, 2U,
          &plan, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report, &saved, sizeof(report)) == 0);
    plan.epochs = 1U;
    memcpy(items[1].originalSourceSha256, items[0].originalSourceSha256,
           UMI_SHA256_BYTES);
    CHECK(UmiAiTrainingStackedExperimentValidate(model, items,
          2U, &plan) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiTrainingStackedExperimentRun(model, items, 2U,
          &plan, &report) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&report, &saved, sizeof(report)) == 0);
    TestStackedSourceDigest(items[1].originalSourceSha256, 8U);
    items[1].partition = UMI_AI_TRAINING_PARTITION_TRAIN;
    CHECK(UmiAiTrainingStackedExperimentValidate(model, items,
          2U, &plan) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiTrainingStackedDecoderInspect(model, &info) == UMI_STATUS_OK);
    CHECK(info.completedTrainingSteps == 0U);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("held-out source leakage and invalid training plan rejected");
    return 0;
}
