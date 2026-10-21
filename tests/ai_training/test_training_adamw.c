/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_adamw.c
 * PURPOSE: Validate bias correction, decoupled decay and atomic rejection.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/optimizer.h"
#include <math.h>
#include <stdio.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
static int Close(float a, float b) { return fabsf(a - b) < 0.00005F; }

int main(void)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *weight = NULL;
    UmiAiTrainingAdamW *adam = NULL, *unchanged = NULL;
    UmiAiTrainingAdamWConfig conf;
    float values[] = {1.0F, -2.0F};
    float grads[] = {0.5F, -0.25F};
    float observed[2] = {0};
    float bad[] = {NAN, 1.0F};
    const float *goodPtrs[] = {grads};
    const float *badPtrs[] = {bad};
    uint64_t steps = 99U;
    size_t bytes = 0U;
    CHECK(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 1U, 2U, values, 2U, 1, &weight) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWDefaults(&conf) == UMI_STATUS_OK);
    conf.weightDecay = 0.1F;
    CHECK(UmiAiTrainingAdamWCreate(tape, &weight, 1U, &conf, &adam) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWInspect(adam, &steps, &bytes) == UMI_STATUS_OK);
    CHECK(steps == 0U && bytes > 0U);
    CHECK(UmiAiTrainingAdamWStepWithGradients(adam, goodPtrs, 0.1F) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(weight, observed, 2U) == UMI_STATUS_OK);
    CHECK(Close(observed[0], 0.89F));
    CHECK(Close(observed[1], -1.88F));
    CHECK(UmiAiTrainingAdamWInspect(adam, &steps, &bytes) == UMI_STATUS_OK);
    CHECK(steps == 1U);
    float before[2] = {observed[0], observed[1]};
    CHECK(UmiAiTrainingAdamWStepWithGradients(adam, badPtrs, 0.1F) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingAdamWInspect(adam, &steps, &bytes) == UMI_STATUS_OK);
    CHECK(steps == 1U);
    CHECK(UmiAiTrainingTensorValues(weight, observed, 2U) == UMI_STATUS_OK);
    CHECK(observed[0] == before[0] && observed[1] == before[1]);
    CHECK(UmiAiTrainingAdamWStepWithGradients(adam, goodPtrs, -0.1F) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingAdamWStepWithGradients(adam, goodPtrs, 0.1F) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWInspect(adam, &steps, &bytes) == UMI_STATUS_OK);
    CHECK(steps == 2U);
    CHECK(UmiAiTrainingTensorValues(weight, observed, 2U) == UMI_STATUS_OK);
    CHECK(observed[0] < before[0] && observed[1] > before[1]);
    conf.beta1 = 1.0F;
    CHECK(UmiAiTrainingAdamWCreate(tape, &weight, 1U, &conf, &unchanged) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(unchanged == NULL);
    UmiAiTrainingAdamWDestroy(adam);
    UmiAiTrainingTapeDestroy(tape);
    puts("native AdamW state, decay and failed-step rollback: PASS");
    return 0;
}
