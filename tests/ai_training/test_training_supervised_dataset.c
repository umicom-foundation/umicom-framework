/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_supervised_dataset.c
 * PURPOSE: Qualify reviewed-source admission through actual native window
 *          construction, immutable copying and reproducible split shuffles.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "supervised_fixture.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "dataset:%d %s\n", __LINE__, #x); return 1; } } while (0)
int main(void)
{
    UmiTestSupervisedFixture fixture;
    UmiAiTrainingSupervisedDataset *dataset = NULL;
    UmiAiTrainingSupervisedDatasetInfo info = {0};
    CHECK(UmiTestSupervisedFixturePrepare(&fixture) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedDatasetCreate(fixture.records, fixture.lengths,
          fixture.evidence, 4U, 8U, &dataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedDatasetInspect(dataset, &info) == UMI_STATUS_OK);
    CHECK(info.trainingExamples == 2U && info.evaluationExamples == 2U);
    CHECK(info.exampleCount == 4U && info.contextLength == 8U);
    CHECK(info.trainingTargets >= 4U && info.evaluationTargets >= 4U);
    CHECK(info.distinctSources == 4U);
    size_t first[4] = {0}, second[4] = {0}, count1 = 0U, count2 = 0U;
    CHECK(UmiAiTrainingSupervisedDatasetOrder(dataset, UMI_AI_TRAINING_PARTITION_TRAIN,
          42U, 7U, first, 4U, &count1) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSupervisedDatasetOrder(dataset, UMI_AI_TRAINING_PARTITION_TRAIN,
          42U, 7U, second, 4U, &count2) == UMI_STATUS_OK);
    CHECK(count1 == 2U && count2 == 2U && memcmp(first, second, 2U * sizeof(size_t)) == 0);
    CHECK(first[0] != first[1] && first[0] < 2U && first[1] < 2U);
    UmiAiTrainingSupervisedExample sample = {0};
    CHECK(UmiAiTrainingSupervisedDatasetGet(dataset, first[0], &sample) == UMI_STATUS_OK);
    CHECK(sample.partition == UMI_AI_TRAINING_PARTITION_TRAIN);
    size_t selected = 0U;
    for (size_t i = 0U; i < 8U; ++i) selected += sample.lossMask[i];
    CHECK(selected == sample.supervisedTokens);
    CHECK(sample.lossMask[0] == 0U && sample.supervisedTokens > 1U);
    /* The returned copy cannot mutate the dataset's retained window. */
    const size_t original = sample.inputIds[0];
    sample.inputIds[0] = 9999U;
    CHECK(UmiAiTrainingSupervisedDatasetGet(dataset, first[0], &sample) == UMI_STATUS_OK);
    CHECK(sample.inputIds[0] == original);
    UmiAiTrainingSupervisedDatasetDestroy(dataset);
    puts("Reviewed JSONL -> immutable response-masked windows -> reproducible split order passed");
    return 0;
}
