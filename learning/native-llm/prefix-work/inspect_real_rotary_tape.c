/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-llm/prefix-work/inspect_real_rotary_tape.c
 * PURPOSE:
 *   Inspect an actual native training tape before and after a rotary attention
 *   forward pass, reverse pass and graph rewind. Report what is measured and
 *   explicitly distinguish tracked tape payloads from total-process memory.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_tape.h"
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define FAIL_IF_ERROR(operation) do { \
    UmiStatus stepStatus = (operation); \
    if (stepStatus != UMI_STATUS_OK) { \
        fprintf(stderr,"Native rotary tape operation failed: %s (status=%d)\n", \
                #operation,(int)stepStatus); \
        goto cleanup; \
    } \
} while (0)

int main(void)
{
    const float queryValues[8] = { .4F,.1F,.3F,.5F, .2F,.8F,.1F,.6F };
    const float keyValues[8] = { .2F,.4F,.8F,.1F, .3F,.7F,.2F,.5F };
    const float valueValues[8] = { .1F,.6F,.3F,.2F, .2F,.7F,.1F,.4F };
    const float targetValues[8] = { .12F,.58F,.29F,.25F, .35F,.55F,.23F,.30F };
    const UmiAiTrainingRotaryConfig rotary = {2U, 2U, 10000.0};
    const uint64_t firstPosition = 17U;
    UmiAiTrainingTape *tape = NULL;
    int exitCode = 1;
    UmiAiTrainingTensor *query = NULL,*key = NULL,*value = NULL,*target = NULL;
    UmiAiTrainingTensor *attended = NULL,*loss = NULL;
    UmiAiTrainingTapeMemory before = {0}, working = {0}, after = {0};
    size_t leafCount = 0U;
    float lossValue = 0.0F, gradients[8] = {0};

    FAIL_IF_ERROR(UmiAiTrainingTapeCreate(&tape));
    /* Every failing step jumps to the same tape-owner cleanup path. */
    FAIL_IF_ERROR(UmiAiTrainingTensorCreate(tape,2U,4U,queryValues,8U,1,&query));
    FAIL_IF_ERROR(UmiAiTrainingTensorCreate(tape,2U,4U,keyValues,8U,1,&key));
    FAIL_IF_ERROR(UmiAiTrainingTensorCreate(tape,2U,4U,valueValues,8U,0,&value));
    FAIL_IF_ERROR(UmiAiTrainingTensorCreate(tape,2U,4U,targetValues,8U,0,&target));
    FAIL_IF_ERROR(UmiAiTrainingTapeCheckpoint(tape,&leafCount));
    FAIL_IF_ERROR(UmiAiTrainingTapeMemoryInspect(tape,&before));
    FAIL_IF_ERROR(UmiAiTrainingRotaryTapeAttention(tape,query,key,value,
                   &rotary,2U,2U,&firstPosition,1U,&attended));
    FAIL_IF_ERROR(UmiAiTrainingMeanSquaredError(tape,attended,target,&loss));
    FAIL_IF_ERROR(UmiAiTrainingTensorValues(loss,&lossValue,1U));
    FAIL_IF_ERROR(UmiAiTrainingBackward(tape,loss));
    FAIL_IF_ERROR(UmiAiTrainingTensorGradients(query,gradients,8U));
    FAIL_IF_ERROR(UmiAiTrainingTapeMemoryInspect(tape,&working));
    FAIL_IF_ERROR(UmiAiTrainingTapeRewind(tape,leafCount));
    FAIL_IF_ERROR(UmiAiTrainingTapeMemoryInspect(tape,&after));
    if (!isfinite(lossValue) || !isfinite(gradients[0]) ||
        working.currentBytes <= before.currentBytes ||
        after.currentBytes != before.currentBytes ||
        after.tensorCount != before.tensorCount) {
        fputs("Native tape evidence failed internal consistency checks.\n",stderr);
        goto cleanup;
    }
    printf("Umicom native C23 rotary tape (production Framework target)\n");
    printf("Tokens: 2; heads: 2; channels/head: 2; first position: %" PRIu64 "\n",
           firstPosition);
    printf("Measured MSE loss: %.9g; first query gradient: %.9g\n",
           (double)lossValue,(double)gradients[0]);
    printf("Committed nodes: leaves=%zu; forward=%zu; after rewind=%zu\n",
           before.tensorCount,working.tensorCount,after.tensorCount);
    printf("Tracked tape memory: leaves=%zu B; working=%zu B; after rewind=%zu B; peak=%zu B\n",
           before.currentBytes,working.currentBytes,after.currentBytes,working.peakBytes);
    puts("Scope: tracked tape payloads, not process RSS or complete LLM throughput.");
    exitCode = 0;
cleanup:
    UmiAiTrainingTapeDestroy(tape);
    return exitCode;
}
