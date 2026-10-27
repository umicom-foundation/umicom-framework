/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_inference_qualification.c
 *
 * PURPOSE:
 *   Qualify real stacked-model K/V and full vocabulary logits across one-,
 *   two- and three-layer growing prefixes and repeated context rollover.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "stacked_incremental_fixture.h"
#include "umicom/ai/training/stacked_inference_qualification.h"

static int QualifyCase(uint32_t layers, uint32_t promptCount)
{
    const UmiAiTrainingStackedDecoderConfig config =
        IncrementalConfig(layers, 5U);
    const size_t prompt[5] = {15U, 23U, 42U, 55U, 65U};
    const size_t next[9] = {66U, 70U, 72U, 75U, 80U, 90U, 95U, 99U, 101U};
    const UmiAiTrainingInferenceParityTolerance tolerance = {
        0.00010F, 0.00010F
    };
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedQualificationReport report = {0};
    VERIFY(UmiAiTrainingStackedDecoderCreate(&config, &model) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedInferenceQualify(model, prompt, promptCount,
        next, 9U, &tolerance, &report) == UMI_STATUS_OK);
    VERIFY(report.initialPromptTokens == promptCount);
    VERIFY(report.continuationTokens == 9U);
    VERIFY(report.transformerLayers == (size_t)layers);
    VERIFY(report.contextLength == 5U);
    VERIFY(report.growingPrefixSteps == 5U - (size_t)promptCount);
    VERIFY(report.fullWindowRebuilds == 9U - report.growingPrefixSteps);
    VERIFY(report.independentCanonicalLogitsCompared >= 5U * 260U);
    VERIFY(report.incrementalLogitsCompared ==
        report.growingPrefixSteps * 260U);
    VERIFY(report.incrementalKeyValueScalarsCompared ==
        report.growingPrefixSteps * (size_t)layers * 8U * 2U);
    VERIFY(report.retainedKeyValueScalarsCompared > 0U);
    VERIFY(report.maximumLogitDifference <= 0.001);
    VERIFY(report.maximumKeyValueDifference <= 0.001);
    UmiAiTrainingStackedDecoderDestroy(model);
    return 0;
}

int main(void)
{
    VERIFY(QualifyCase(1U, 1U) == 0);
    VERIFY(QualifyCase(2U, 3U) == 0);
    VERIFY(QualifyCase(3U, 5U) == 0);
    puts("Native qualification: every active logit and K/V row agrees");
    return 0;
}
