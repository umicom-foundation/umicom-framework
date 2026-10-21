/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_audit_changes.c
 *
 * PURPOSE: Detect changed training settings and model state without writing
 *          or implicitly trusting a new checkpoint.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/model_audit.h"
#include "umicom/ai/training/checkpoint.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef UMI_AI_TEST_MANIFEST_PATH
#error UMI_AI_TEST_MANIFEST_PATH must be supplied by the build system
#endif
#ifndef UMI_AI_TEST_ALTERNATIVE_MANIFEST_PATH
#error UMI_AI_TEST_ALTERNATIVE_MANIFEST_PATH must be supplied by the build system
#endif
#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
    return 1; } } while (0)

int main(void)
{
    UmiAiTrainingDataset *dataset = NULL, *alternateDataset = NULL;
    UmiAiTrainingDecoder *model = NULL, *restored = NULL;
    UmiAiTrainingExperimentConfig config, altered;
    UmiAiTrainingModelAudit evidence = {0}, updated = {0};
    uint8_t *snapshot = NULL;
    size_t bytes = 0U, written = 0U;
    size_t inputs[8], targets[8];
    float loss = -1.0F;
    int matches = -1;

    CHECK(UmiAiTrainingExperimentDefaults(&config) == UMI_STATUS_OK);
    config.steps = 4U;
    config.evalInterval = 2U;
    config.evalBatches = 2U;
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_MANIFEST_PATH,
                                           &dataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&config.decoder, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingModelAuditEvaluate(model, dataset, &config,
                                           &evidence) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointSize(model, &bytes) == UMI_STATUS_OK);
    snapshot = malloc(bytes);
    CHECK(snapshot != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, snapshot,
                                               bytes, &written) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointDecode(snapshot, written,
                                                &restored) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingModelAuditVerify(&evidence, restored, dataset,
                                      &config, &matches) == UMI_STATUS_OK);
    CHECK(matches == 1);
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_ALTERNATIVE_MANIFEST_PATH,
                                           &alternateDataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingModelAuditVerify(&evidence, restored, alternateDataset,
                                      &config, &matches) == UMI_STATUS_OK);
    CHECK(matches == 0);
    altered = config;
    altered.learningRate = 0.05F;
    CHECK(UmiAiTrainingModelAuditVerify(&evidence, restored, dataset,
                                      &altered, &matches) == UMI_STATUS_OK);
    CHECK(matches == 0);
    altered = config;
    altered.evalSeed++;
    CHECK(UmiAiTrainingModelAuditVerify(&evidence, restored, dataset,
                                      &altered, &matches) == UMI_STATUS_OK);
    CHECK(matches == 0);
    altered = config;
    altered.steps++;
    CHECK(UmiAiTrainingModelAuditVerify(&evidence, restored, dataset,
                                      &altered, &matches) == UMI_STATUS_OK);
    CHECK(matches == 0);
    CHECK(UmiAiTrainingDatasetBatch(dataset, UMI_AI_TRAINING_PARTITION_TRAIN,
             8U, 1U, config.trainSeed, 0U, inputs, targets, 8U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderTrainStep(restored, inputs, targets, 8U,
             config.learningRate, &loss) == UMI_STATUS_OK);
    CHECK(isfinite(loss));
    CHECK(UmiAiTrainingModelAuditVerify(&evidence, restored, dataset,
                                      &config, &matches) == UMI_STATUS_OK);
    CHECK(matches == 0);
    CHECK(UmiAiTrainingModelAuditEvaluate(restored, dataset, &config,
                                           &updated) == UMI_STATUS_OK);
    CHECK(updated.modelFingerprint != evidence.modelFingerprint);
    CHECK(updated.datasetFingerprint == evidence.datasetFingerprint);
    CHECK(updated.planFingerprint == evidence.planFingerprint);
    CHECK(updated.completedSteps == 1U);
    free(snapshot);
    UmiAiTrainingDecoderDestroy(restored);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDatasetDestroy(alternateDataset);
    UmiAiTrainingDatasetDestroy(dataset);
    puts("restored checkpoint matches; changed plan and weights do not: PASS");
    return 0;
}
