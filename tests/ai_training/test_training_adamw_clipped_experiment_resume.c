/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_adamw_clipped_experiment_resume.c
 * PURPOSE: Prove bit-identical AdamW model+moments after clipping-aware
 *          continuation and unchanged original checkpoint handles.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * LICENCE: MIT
 */
#include "umicom/ai/training/experiment_adamw.h"
#include "umicom/ai/training/optimizer_checkpoint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef UMI_AI_TEST_MANIFEST_PATH
#error UMI_AI_TEST_MANIFEST_PATH required
#endif
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

static int IdenticalCheckpoint(const UmiAiTrainingDecoder *a,
                               const UmiAiTrainingAdamW *ao,
                               const UmiAiTrainingDecoder *b,
                               const UmiAiTrainingAdamW *bo)
{
    size_t sa = 0U, sb = 0U, wa = 0U, wb = 0U;
    uint8_t *ab = NULL, *bb = NULL;
    CHECK(UmiAiTrainingAdamWCheckpointSize(a, ao, &sa) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWCheckpointSize(b, bo, &sb) == UMI_STATUS_OK);
    CHECK(sa == sb && sa != 0U);
    ab = malloc(sa); bb = malloc(sb);
    CHECK(ab != NULL && bb != NULL);
    CHECK(UmiAiTrainingAdamWCheckpointEncode(a, ao, ab, sa, &wa) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWCheckpointEncode(b, bo, bb, sb, &wb) == UMI_STATUS_OK);
    CHECK(wa == sa && wb == sb && memcmp(ab, bb, sa) == 0);
    free(ab); free(bb);
    return 0;
}

int main(void)
{
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *full = NULL, *partial = NULL, *resumed = NULL;
    UmiAiTrainingAdamW *fo = NULL, *po = NULL, *ro = NULL;
    UmiAiTrainingExperimentConfig config, intermediate;
    UmiAiTrainingAdamWConfig options;
    UmiAiTrainingLearningRateSchedule schedule;
    UmiAiTrainingExperimentResult fr, pr, rr;
    UmiAiTrainingGradientClipReport fc, pc, rc, unchanged;
    uint64_t steps = 0U;
    size_t stateBytes = 0U;
    CHECK(UmiAiTrainingExperimentDefaults(&config) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWDefaults(&options) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetLoadManifest(
        UMI_AI_TEST_MANIFEST_PATH, &dataset) == UMI_STATUS_OK);
    config.steps = 4U;
    config.evalInterval = 2U;
    config.evalBatches = 1U;
    config.batchSequences = 2U;
    config.learningRate = 0.016F;
    schedule = (UmiAiTrainingLearningRateSchedule){
        UMI_AI_TRAINING_SCHEDULE_COSINE, 0.016F, 0.003F, 1U, 4U};
    CHECK(UmiAiTrainingExperimentRunAdamWClipped(
        dataset, &config, &options, &schedule, 1U, 0.01F,
        NULL, NULL, &full, &fo, &fr, &fc) == UMI_STATUS_OK);
    intermediate = config;
    intermediate.steps = 2U;
    CHECK(UmiAiTrainingExperimentRunAdamWClipped(
        dataset, &intermediate, &options, &schedule, 1U, 0.01F,
        NULL, NULL, &partial, &po, &pr, &pc) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingExperimentResumeAdamWClipped(
        dataset, &config, &options, &schedule, partial, po, 1U, 0.01F,
        NULL, NULL, &resumed, &ro, &rr, &rc) == UMI_STATUS_OK);
    CHECK(fr.completedSteps == 4U && pr.completedSteps == 2U && rr.completedSteps == 4U);
    CHECK(fc.appliedScale <= 1.0 && rc.appliedScale <= 1.0);
    CHECK(IdenticalCheckpoint(full, fo, resumed, ro) == 0);
    CHECK(UmiAiTrainingAdamWInspect(po, &steps, &stateBytes) == UMI_STATUS_OK);
    CHECK(steps == 2U && stateBytes > 0U);
    memset(&unchanged, 0x74, sizeof(unchanged));
    pc = unchanged;
    CHECK(UmiAiTrainingExperimentResumeAdamWClipped(
        dataset, &config, &options, &schedule, partial, po, 0U, 0.01F,
        NULL, NULL, &resumed, &ro, &rr, &pc) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&pc, &unchanged, sizeof(pc)) == 0);
    UmiAiTrainingAdamWDestroy(ro); UmiAiTrainingAdamWDestroy(po);
    UmiAiTrainingAdamWDestroy(fo);
    UmiAiTrainingDecoderDestroy(resumed); UmiAiTrainingDecoderDestroy(partial);
    UmiAiTrainingDecoderDestroy(full);
    UmiAiTrainingDatasetDestroy(dataset);
    puts("clipped AdamW experiment: exact combined continuation and atomic outputs PASS");
    return 0;
}
