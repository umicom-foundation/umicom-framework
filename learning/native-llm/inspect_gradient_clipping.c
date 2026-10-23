/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM Learning Laboratory
 * File: learning/native-llm/inspect_gradient_clipping.c
 * PURPOSE: Observe global gradient clipping before one native AdamW update.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * LICENCE: MIT
 */
#include "umicom/ai/training/gradient_clipping.h"
#include "umicom/ai/training/optimizer.h"

#include <stdio.h>

int main(void)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *parameter = NULL;
    UmiAiTrainingAdamW *optimizer = NULL;
    const UmiAiTrainingAdamWConfig options = {0.0F, 0.0F, 1.0F, 0.0F};
    const float initial[] = {0.0F, 0.0F};
    const float gradient[] = {3.0F, 4.0F};
    const float *arrays[] = {gradient};
    UmiAiTrainingGradientClipReport report = {0};
    UmiStatus status;
    float updated[2] = {0.0F, 0.0F};

    status = UmiAiTrainingTapeCreate(&tape);
    if (status != UMI_STATUS_OK) goto failure;
    status = UmiAiTrainingTensorCreate(tape, 1U, 2U, initial, 2U, 1,
                                       &parameter);
    if (status != UMI_STATUS_OK) goto failure;
    status = UmiAiTrainingAdamWCreate(tape, &parameter, 1U, &options, &optimizer);
    if (status != UMI_STATUS_OK) goto failure;
    status = UmiAiTrainingAdamWStepWithClippedGradients(
        optimizer, arrays, 0.1F, 2.5F, &report);
    if (status != UMI_STATUS_OK) goto failure;
    status = UmiAiTrainingTensorValues(parameter, updated, 2U);
    if (status != UMI_STATUS_OK) goto failure;

    puts("Global FP32 gradient norm and AdamW clipping");
    printf("Norm before clipping: %.6f\n", report.globalNorm);
    printf("Clipping factor: %.6f\n", report.appliedScale);
    printf("Gradient entries: %zu\n", report.gradientElements);
    printf("Updated parameters: %.6f, %.6f\n",
           (double)updated[0], (double)updated[1]);
    UmiAiTrainingAdamWDestroy(optimizer);
    UmiAiTrainingTapeDestroy(tape);
    return 0;

failure:
    fprintf(stderr, "Gradient clipping demo failed (status=%d)\n", (int)status);
    UmiAiTrainingAdamWDestroy(optimizer);
    UmiAiTrainingTapeDestroy(tape);
    return 1;
}
