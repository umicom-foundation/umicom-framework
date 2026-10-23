/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_learning_rate.c
 * PURPOSE: Verify scheduled learning rates, warmup and failure-atomic outputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * LICENCE: MIT
 */
#include "umicom/ai/training/optimizer.h"
#include <math.h>
#include <stdio.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
static int Close(float a, float b) { return fabsf(a - b) < 0.000001F; }

int main(void)
{
    UmiAiTrainingLearningRateSchedule schedule = {
        UMI_AI_TRAINING_SCHEDULE_COSINE, 0.1F, 0.01F, 2U, 6U};
    float value = -49.0F;
    CHECK(UmiAiTrainingLearningRateForStep(&schedule, 1U, &value) == UMI_STATUS_OK);
    CHECK(Close(value, 0.05F));
    CHECK(UmiAiTrainingLearningRateForStep(&schedule, 2U, &value) == UMI_STATUS_OK);
    CHECK(Close(value, 0.1F));
    CHECK(UmiAiTrainingLearningRateForStep(&schedule, 4U, &value) == UMI_STATUS_OK);
    CHECK(Close(value, 0.055F));
    CHECK(UmiAiTrainingLearningRateForStep(&schedule, 6U, &value) == UMI_STATUS_OK);
    CHECK(Close(value, 0.01F));
    schedule.kind = UMI_AI_TRAINING_SCHEDULE_LINEAR;
    CHECK(UmiAiTrainingLearningRateForStep(&schedule, 4U, &value) == UMI_STATUS_OK);
    CHECK(Close(value, 0.055F));
    schedule.kind = UMI_AI_TRAINING_SCHEDULE_CONSTANT;
    CHECK(UmiAiTrainingLearningRateForStep(&schedule, 6U, &value) == UMI_STATUS_OK);
    CHECK(Close(value, 0.1F));
    value = -17.0F;
    CHECK(UmiAiTrainingLearningRateForStep(&schedule, 7U, &value) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(value == -17.0F);
    schedule.warmupSteps = 6U;
    CHECK(UmiAiTrainingLearningRateValidate(&schedule) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingLearningRateForStep(&schedule, 1U, &value) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(value == -17.0F);
    schedule.warmupSteps = 0U;
    schedule.peakLearningRate = NAN;
    CHECK(UmiAiTrainingLearningRateValidate(&schedule) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingLearningRateForStep(NULL, 1U, &value) == UMI_STATUS_INVALID_ARGUMENT);
    puts("native learning-rate schedule: PASS");
    return 0;
}
