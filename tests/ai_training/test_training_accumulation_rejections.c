/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_accumulation_rejections.c
 * PURPOSE: Reject unsupported microbatches, preserve model/report outputs
 *          after cancellation, and leave an input checkpoint untouched.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/experiment_accumulation.h"
#include "umicom/ai/training/experiment_plan.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#ifndef UMI_AI_TEST_ACCUMULATED_PLAN_PATH
#error UMI_AI_TEST_ACCUMULATED_PLAN_PATH required by CMake
#endif
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

static UmiStatus CancelAtBaseline(const UmiAiTrainingExperimentProgress *p,
                                  void *userData)
{
    (void)userData;
    return p == NULL ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_CANCELLED;
}

int main(void)
{
    UmiAiTrainingExperimentConfig config;
    UmiAiTrainingExperimentAccumulation options;
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *existing = NULL, *outModel = NULL;
    UmiAiTrainingExperimentResult result, oldResult;
    UmiAiTrainingDecoderConfig inspectedConfig;
    uint64_t initialSteps = 400U;
    char manifest[1024];
    CHECK(UmiAiTrainingExperimentPlanLoadAccumulated(
        UMI_AI_TEST_ACCUMULATED_PLAN_PATH, &config,
        &options, manifest, sizeof(manifest)) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingExperimentAccumulationValidate(&config,
        &options) == UMI_STATUS_OK);
    UmiAiTrainingExperimentAccumulation invalid = options;
    invalid.microbatchSequences = invalid.effectiveSequences;
    CHECK(UmiAiTrainingExperimentAccumulationValidate(&config,
        &invalid) == UMI_STATUS_CAPACITY_EXCEEDED);
    invalid = options;
    invalid.effectiveSequences = 0U;
    CHECK(UmiAiTrainingExperimentAccumulationValidate(&config,
        &invalid) == UMI_STATUS_INVALID_ARGUMENT);
    invalid = options;
    invalid.effectiveSequences = UMI_AI_EXPERIMENT_MAX_EFFECTIVE_SEQUENCES + 1U;
    CHECK(UmiAiTrainingExperimentAccumulationValidate(&config,
        &invalid) == UMI_STATUS_INVALID_ARGUMENT);
    invalid = options;
    invalid.microbatchSequences = invalid.effectiveSequences + 1U;
    CHECK(UmiAiTrainingExperimentAccumulationValidate(&config,
        &invalid) == UMI_STATUS_INVALID_ARGUMENT);

    CHECK(UmiAiTrainingDatasetLoadManifest(manifest, &dataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&config.decoder, &existing) == UMI_STATUS_OK);
    memset(&result, 0xA6, sizeof(result));
    oldResult = result;
    outModel = NULL;
    CHECK(UmiAiTrainingExperimentRunAccumulated(dataset, &config, &options,
        CancelAtBaseline, NULL, &outModel, &result) == UMI_STATUS_CANCELLED);
    CHECK(outModel == NULL && memcmp(&result, &oldResult, sizeof(result)) == 0);
    CHECK(UmiAiTrainingExperimentResumeAccumulated(dataset, &config,
        &options, existing, CancelAtBaseline, NULL, &outModel,
        &result) == UMI_STATUS_CANCELLED);
    CHECK(outModel == NULL && memcmp(&result, &oldResult, sizeof(result)) == 0);
    CHECK(UmiAiTrainingDecoderInspect(existing, &inspectedConfig,
        &initialSteps) == UMI_STATUS_OK && initialSteps == 0U);
    CHECK(UmiAiTrainingExperimentRunAccumulated(dataset, &config, NULL,
        NULL, NULL, &outModel, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(outModel == NULL && memcmp(&result, &oldResult, sizeof(result)) == 0);
    UmiAiTrainingDecoderDestroy(existing);
    UmiAiTrainingDatasetDestroy(dataset);
    puts("Accumulation limits, cancellation and failure-atomic outputs: PASS");
    return 0;
}
