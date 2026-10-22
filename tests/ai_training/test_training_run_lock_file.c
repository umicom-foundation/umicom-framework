/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_run_lock_file.c
 * PURPOSE: Prove no-clobber creation, tamper rejection, and missing companion
 *          refusal for reviewed native AdamW run locks.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/training_run_lock.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

#ifndef UMI_AI_TEST_MANIFEST_PATH
#error UMI_AI_TEST_MANIFEST_PATH must be provided by CMake
#endif
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

int main(void)
{
#if defined(_WIN32)
    const unsigned long pid = (unsigned long)GetCurrentProcessId();
#else
    const unsigned long pid = (unsigned long)getpid();
#endif
    const uint8_t checkpoint[] = {0x20U, 0x22U, 0x45U};
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingExperimentConfig config;
    UmiAiTrainingAdamWConfig adamw;
    UmiAiTrainingGradientPolicy clip = {1.0F, 2U, 1U};
    UmiAiTrainingLearningRateSchedule schedule;
    UmiAiTrainingRunLockInputs input;
    char path[128];
    FILE *file = NULL;
    int nameLength;

    CHECK(UmiAiTrainingExperimentDefaults(&config) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWDefaults(&adamw) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_MANIFEST_PATH,
          &dataset) == UMI_STATUS_OK);
    config.steps = 4U;
    config.evalInterval = 2U;
    config.evalBatches = 1U;
    config.batchSequences = 2U;
    config.learningRate = 0.016F;
    schedule = (UmiAiTrainingLearningRateSchedule){
        UMI_AI_TRAINING_SCHEDULE_COSINE, 0.016F, 0.003F, 2U, 8U};
    input = (UmiAiTrainingRunLockInputs){
        dataset, &config, &adamw, &schedule, &clip};
    nameLength = snprintf(path, sizeof(path),
                          "umicom-adamw-run-lock-test-%lu.training-lock", pid);
    CHECK(nameLength > 0 && (size_t)nameLength < sizeof(path));
    CHECK(UmiAiTrainingRunLockVerifyFile(path, checkpoint,
          sizeof(checkpoint), &input) == UMI_STATUS_IO_ERROR);
    CHECK(UmiAiTrainingRunLockSaveNewFile(path, checkpoint,
          sizeof(checkpoint), &input) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingRunLockSaveNewFile(path, checkpoint,
          sizeof(checkpoint), &input) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(UmiAiTrainingRunLockVerifyFile(path, checkpoint,
          sizeof(checkpoint), &input) == UMI_STATUS_OK);
    file = fopen(path, "ab");
    CHECK(file != NULL && fputc('!', file) != EOF);
    CHECK(fclose(file) == 0);
    CHECK(UmiAiTrainingRunLockVerifyFile(path, checkpoint,
          sizeof(checkpoint), &input) == UMI_STATUS_PARSE_ERROR);
    CHECK(remove(path) == 0);
    UmiAiTrainingDatasetDestroy(dataset);
    puts("AdamW run lock companion: no-clobber and tamper rejection PASS");
    return 0;
}
