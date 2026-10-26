/* Umicom Framework | Held-out model selection | Sammy Hegab | MIT */
#include "stacked_inference_fixture.h"
#include "umicom/ai/training/stacked_model_selection.h"
int main(void)
{
    UmiAiTrainingStackedDecoder *reference = NULL, *candidate = NULL;
    const size_t inputs[] = {1U, 109U, 114U, 120U};
    size_t targets[] = {0U, 0U, 0U, 0U};
    const uint8_t mask[] = {0U, 0U, 0U, 1U};
    float logits[4U * 260U] = {0};
    UmiAiTrainingStackedEvaluationCase item = Example(inputs, targets, mask);
    UmiAiTrainingStackedSelectionPolicy policy = {0.0, 0.0};
    UmiAiTrainingStackedSelectionReport report = {0};
    CHECK(MakeModel(UINT64_C(14), &reference) == UMI_STATUS_OK);
    CHECK(MakeModel(UINT64_C(14), &candidate) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderLogits(reference, inputs, 4U,
        logits, 4U * 260U) == UMI_STATUS_OK);
    size_t best = 0U;
    for (size_t j = 1U; j < 260U; ++j)
        if (logits[3U * 260U + j] > logits[3U * 260U + best]) best = j;
    targets[3] = (best + 1U) % 260U; /* Guaranteed held-out top-one miss. */
    CHECK(UmiAiTrainingStackedModelSelectionCompare(reference, candidate,
        &item, 1U, &policy, &report) == UMI_STATUS_OK);
    CHECK(report.candidateAccepted == 1 && report.lossAccepted == 1);
    policy.minimumTopOneAccuracy = 1.0;
    CHECK(UmiAiTrainingStackedModelSelectionCompare(reference, candidate,
        &item, 1U, &policy, &report) == UMI_STATUS_OK);
    CHECK(report.accuracyAccepted == 0 && report.candidateAccepted == 0);
    policy.maximumAllowedLossIncrease = -1.0;
    UmiAiTrainingStackedSelectionReport before = report;
    CHECK(UmiAiTrainingStackedModelSelectionCompare(reference, candidate,
        &item, 1U, &policy, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report, &before, sizeof(report)) == 0);
    UmiAiTrainingStackedDecoderDestroy(candidate);
    UmiAiTrainingStackedDecoderDestroy(reference);
    puts("stacked model selection: accepted/rejected outcomes separated");
    return 0;
}
