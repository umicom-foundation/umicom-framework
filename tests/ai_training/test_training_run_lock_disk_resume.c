/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_run_lock_disk_resume.c
 *
 * PURPOSE:
 *   Qualify a REAL on-disk decoder+AdamW checkpoint round-trip, independent
 *   clipping and run-policy sidecars, and byte-identical continued training.
 *   Unlike the earlier in-memory continuation test, this uses the filesystem
 *   checkpoint codec which users actually depend on between application runs.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_run_lock.h"
#include "umicom/ai/training/optimizer_checkpoint.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

#ifndef UMI_AI_TEST_MANIFEST_PATH
#error UMI_AI_TEST_MANIFEST_PATH must be provided by CMake
#endif
#ifndef UMI_AI_TEST_REORDERED_MANIFEST_PATH
#error UMI_AI_TEST_REORDERED_MANIFEST_PATH must be provided by CMake
#endif
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

static int EncodePair(const UmiAiTrainingDecoder *model,
                      const UmiAiTrainingAdamW *optimiser,
                      uint8_t **outBytes, size_t *outSize)
{
    size_t capacity = 0U, written = 0U;
    uint8_t *bytes;
    CHECK(UmiAiTrainingAdamWCheckpointSize(model, optimiser, &capacity) ==
          UMI_STATUS_OK);
    CHECK(capacity > 0U && capacity <= UMI_AI_ADAMW_MAX_CHECKPOINT_BYTES);
    bytes = malloc(capacity);
    CHECK(bytes != NULL);
    CHECK(UmiAiTrainingAdamWCheckpointEncode(model, optimiser,
          bytes, capacity, &written) == UMI_STATUS_OK);
    CHECK(written == capacity);
    *outBytes = bytes;
    *outSize = capacity;
    return 0;
}

int main(void)
{
#if defined(_WIN32)
    const unsigned long pid = (unsigned long)GetCurrentProcessId();
#else
    const unsigned long pid = (unsigned long)getpid();
#endif
    UmiAiTrainingDataset *dataset = NULL, *reordered = NULL;
    UmiAiTrainingDecoder *full = NULL, *partial = NULL;
    UmiAiTrainingDecoder *loaded = NULL, *resumed = NULL;
    UmiAiTrainingAdamW *fullAdamW = NULL, *partialAdamW = NULL;
    UmiAiTrainingAdamW *loadedAdamW = NULL, *resumedAdamW = NULL;
    UmiAiTrainingExperimentConfig config, intermediate;
    UmiAiTrainingAdamWConfig optimizer;
    UmiAiTrainingLearningRateSchedule schedule;
    UmiAiTrainingGradientPolicy clipping = {0.01F, 2U, 1U};
    UmiAiTrainingRunLockInputs identity;
    UmiAiTrainingGradientClipReport fullClip, partialClip, resumedClip;
    UmiAiTrainingExperimentResult fullReport, partialReport, resumedReport;
    uint8_t *checkpointBytes = NULL, *fullBytes = NULL;
    uint8_t *loadedBytes = NULL, *resumedBytes = NULL;
    size_t checkpointSize = 0U, fullSize = 0U;
    size_t loadedSize = 0U, resumedSize = 0U;
    uint64_t completed = 0U;
    size_t momentBytes = 0U;
    char path[128], clipPath[160], lockPath[160];
    int length;

    CHECK(UmiAiTrainingExperimentDefaults(&config) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWDefaults(&optimizer) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_MANIFEST_PATH,
          &dataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetLoadManifest(
          UMI_AI_TEST_REORDERED_MANIFEST_PATH, &reordered) == UMI_STATUS_OK);
    config.steps = 4U;
    config.evalInterval = 2U;
    config.evalBatches = 1U;
    config.batchSequences = 2U;
    config.learningRate = 0.016F;
    schedule = (UmiAiTrainingLearningRateSchedule){
        UMI_AI_TRAINING_SCHEDULE_COSINE, 0.016F, 0.003F, 1U, 4U};
    identity = (UmiAiTrainingRunLockInputs){
        dataset, &config, &optimizer, &schedule, &clipping};
    length = snprintf(path, sizeof(path), "umicom-run-lock-%lu.umi", pid);
    CHECK(length > 0 && (size_t)length < sizeof(path));
    length = snprintf(clipPath, sizeof(clipPath), "%s.clip-policy", path);
    CHECK(length > 0 && (size_t)length < sizeof(clipPath));
    length = snprintf(lockPath, sizeof(lockPath), "%s.training-lock", path);
    CHECK(length > 0 && (size_t)length < sizeof(lockPath));

    /* Same immutable dataset, same absolute schedule and optimiser settings.
     * Only the intentional end step differs before versus after resuming. */
    CHECK(UmiAiTrainingExperimentRunAdamWClipped(dataset, &config,
          &optimizer, &schedule, 1U, clipping.maximumGradientNorm,
          NULL, NULL, &full, &fullAdamW, &fullReport, &fullClip) == UMI_STATUS_OK);
    intermediate = config;
    intermediate.steps = 2U;
    CHECK(UmiAiTrainingExperimentRunAdamWClipped(dataset, &intermediate,
          &optimizer, &schedule, 1U, clipping.maximumGradientNorm,
          NULL, NULL, &partial, &partialAdamW, &partialReport, &partialClip) ==
          UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWCheckpointSaveNewFile(
          partial, partialAdamW, path) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWCheckpointLoadFile(
          path, &loaded, &loadedAdamW) == UMI_STATUS_OK);
    CHECK(EncodePair(partial, partialAdamW,
                     &checkpointBytes, &checkpointSize) == 0);
    CHECK(EncodePair(loaded, loadedAdamW,
                     &loadedBytes, &loadedSize) == 0);
    CHECK(checkpointSize == loadedSize);
    CHECK(memcmp(checkpointBytes, loadedBytes, checkpointSize) == 0);
    CHECK(UmiAiTrainingGradientPolicySaveNewFile(clipPath,
          checkpointBytes, checkpointSize, &clipping) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingRunLockSaveNewFile(lockPath,
          checkpointBytes, checkpointSize, &identity) == UMI_STATUS_OK);

    /* Both independent records MUST verify before any resumed optimisation. */
    CHECK(UmiAiTrainingGradientPolicyVerifyFile(clipPath,
          loadedBytes, loadedSize, &clipping) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingRunLockVerifyFile(lockPath,
          loadedBytes, loadedSize, &identity) == UMI_STATUS_OK);
    identity.dataset = reordered;
    CHECK(UmiAiTrainingRunLockVerifyFile(lockPath,
          loadedBytes, loadedSize, &identity) == UMI_STATUS_PARSE_ERROR);
    identity.dataset = dataset;
    schedule.totalSteps++;
    CHECK(UmiAiTrainingRunLockVerifyFile(lockPath,
          loadedBytes, loadedSize, &identity) == UMI_STATUS_PARSE_ERROR);
    schedule.totalSteps--;
    CHECK(UmiAiTrainingRunLockVerifyFile(lockPath,
          loadedBytes, loadedSize, &identity) == UMI_STATUS_OK);

    CHECK(UmiAiTrainingExperimentResumeAdamWClipped(dataset, &config,
          &optimizer, &schedule, loaded, loadedAdamW,
          1U, clipping.maximumGradientNorm, NULL, NULL,
          &resumed, &resumedAdamW, &resumedReport, &resumedClip) ==
          UMI_STATUS_OK);
    CHECK(fullReport.completedSteps == 4U &&
          partialReport.completedSteps == 2U &&
          resumedReport.completedSteps == 4U);
    CHECK(EncodePair(full, fullAdamW, &fullBytes, &fullSize) == 0);
    CHECK(EncodePair(resumed, resumedAdamW, &resumedBytes, &resumedSize) == 0);
    CHECK(fullSize == resumedSize);
    CHECK(memcmp(fullBytes, resumedBytes, fullSize) == 0);

    /* The loaded checkpoint is BORROWED by resume, never advanced or mutated. */
    CHECK(UmiAiTrainingAdamWInspect(loadedAdamW,
          &completed, &momentBytes) == UMI_STATUS_OK);
    CHECK(completed == 2U && momentBytes > 0U);
    free(loadedBytes);
    loadedBytes = NULL;
    CHECK(EncodePair(loaded, loadedAdamW,
                     &loadedBytes, &loadedSize) == 0);
    CHECK(loadedSize == checkpointSize);
    CHECK(memcmp(loadedBytes, checkpointBytes, checkpointSize) == 0);
    CHECK(UmiAiTrainingRunLockVerifyFile(lockPath,
          loadedBytes, loadedSize, &identity) == UMI_STATUS_OK);
    CHECK(remove(lockPath) == 0);
    CHECK(UmiAiTrainingRunLockVerifyFile(lockPath,
          loadedBytes, loadedSize, &identity) == UMI_STATUS_IO_ERROR);
    CHECK(remove(clipPath) == 0);
    CHECK(remove(path) == 0);

    free(resumedBytes); free(fullBytes);
    free(loadedBytes); free(checkpointBytes);
    UmiAiTrainingAdamWDestroy(resumedAdamW);
    UmiAiTrainingAdamWDestroy(loadedAdamW);
    UmiAiTrainingAdamWDestroy(partialAdamW);
    UmiAiTrainingAdamWDestroy(fullAdamW);
    UmiAiTrainingDecoderDestroy(resumed);
    UmiAiTrainingDecoderDestroy(loaded);
    UmiAiTrainingDecoderDestroy(partial);
    UmiAiTrainingDecoderDestroy(full);
    UmiAiTrainingDatasetDestroy(reordered);
    UmiAiTrainingDatasetDestroy(dataset);
    puts("AdamW disk checkpoint, independent policy files and exact resume PASS");
    return 0;
}
