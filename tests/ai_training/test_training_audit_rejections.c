/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_audit_rejections.c
 *
 * PURPOSE: Verify failure-atomic reports, invalid inputs and plan mismatch.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/model_audit.h"
#include <stdio.h>
#include <string.h>

#ifndef UMI_AI_TEST_MANIFEST_PATH
#error UMI_AI_TEST_MANIFEST_PATH must be supplied by the build system
#endif
#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
    return 1; } } while (0)

int main(void)
{
    UmiAiTrainingExperimentConfig config, wrong;
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingModelAudit report, preserved;
    int matches = 7;

    CHECK(UmiAiTrainingExperimentDefaults(&config) == UMI_STATUS_OK);
    config.steps = 2U;
    config.evalInterval = 1U;
    config.evalBatches = 1U;
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_MANIFEST_PATH,
                                           &dataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&config.decoder, &model) == UMI_STATUS_OK);
    memset(&report, 0xA5, sizeof(report));
    memcpy(&preserved, &report, sizeof(preserved));
    CHECK(UmiAiTrainingModelAuditEvaluate(NULL, dataset, &config,
                                           &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingModelAuditEvaluate(model, NULL, &config,
                                           &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingModelAuditEvaluate(model, dataset, NULL,
                                           &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingModelAuditEvaluate(model, dataset, &config,
                                           NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report, &preserved, sizeof(report)) == 0);
    wrong = config;
    wrong.decoder.initialSeed++;
    CHECK(UmiAiTrainingModelAuditEvaluate(model, dataset, &wrong,
                                           &report) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&report, &preserved, sizeof(report)) == 0);
    wrong = config;
    wrong.evalBatches = 0U;
    CHECK(UmiAiTrainingModelAuditEvaluate(model, dataset, &wrong,
                                           &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report, &preserved, sizeof(report)) == 0);
    CHECK(UmiAiTrainingModelAuditVerify(NULL, model, dataset,
                                      &config, &matches) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(matches == 7);
    CHECK(UmiAiTrainingModelAuditVerify(&preserved, model, dataset,
                                      NULL, &matches) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(matches == 7);
    CHECK(UmiAiTrainingModelAuditEvaluate(model, dataset, &config,
                                           &report) == UMI_STATUS_OK);
    wrong = config;
    wrong.decoder.initialSeed++;
    CHECK(UmiAiTrainingModelAuditVerify(&report, model, dataset,
                                      &wrong, &matches) == UMI_STATUS_OK);
    CHECK(matches == 0);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDatasetDestroy(dataset);
    puts("invalid and mismatched audit requests preserve caller outputs: PASS");
    return 0;
}
