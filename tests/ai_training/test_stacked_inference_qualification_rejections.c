/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_inference_qualification_rejections.c
 *
 * PURPOSE:
 *   Ensure rejected prompts, invalid tolerances and continuation IDs never
 *   publish partial results from native inference qualification.
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

int main(void)
{
    const UmiAiTrainingStackedDecoderConfig config = IncrementalConfig(2U, 5U);
    const size_t prompt[] = {20U, 21U};
    const size_t badPrompt[] = {20U, 999U};
    const size_t next[] = {22U, 23U};
    const size_t invalidToken[] = {999U};
    const UmiAiTrainingInferenceParityTolerance tolerance = {
        0.0001F, 0.0001F
    };
    const UmiAiTrainingInferenceParityTolerance invalidTolerance = {
        -1.0F, 0.0F
    };
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedQualificationReport report;
    memset(&report, 0xa5, sizeof(report));
    const UmiAiTrainingStackedQualificationReport untouched = report;
    VERIFY(UmiAiTrainingStackedDecoderCreate(&config, &model) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedInferenceQualify(model, prompt, 0U, next,
        2U, &tolerance, &report) == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(memcmp(&untouched, &report, sizeof(report)) == 0);
    VERIFY(UmiAiTrainingStackedInferenceQualify(model, badPrompt, 2U, next,
        2U, &tolerance, &report) == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(memcmp(&untouched, &report, sizeof(report)) == 0);
    VERIFY(UmiAiTrainingStackedInferenceQualify(model, prompt, 2U,
        invalidToken, 1U, &tolerance, &report) == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(memcmp(&untouched, &report, sizeof(report)) == 0);
    VERIFY(UmiAiTrainingStackedInferenceQualify(model, prompt, 2U,
        next, 2U, &invalidTolerance, &report) == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(memcmp(&untouched, &report, sizeof(report)) == 0);
    VERIFY(UmiAiTrainingStackedInferenceQualify(model, prompt, 2U,
        next, 0U, &tolerance, &report) == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(memcmp(&untouched, &report, sizeof(report)) == 0);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("Native qualification: rejected requests do not publish evidence");
    return 0;
}
