/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_attention.c
 * PURPOSE: Verify causal mask, stable softmax and independent batch sequences.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/tape.h"

#include <math.h>
#include <stdio.h>

#define VERIFY(ok) do { if (!(ok)) { \
    fprintf(stderr, "causal attention verification failed, line %d: %s\n", \
        __LINE__, #ok); return 1; } } while (0)

static int Close(float left, float right)
{
    return fabsf(left - right) < 0.0002F;
}

static UmiStatus Evaluate(const float *q, const float *k, const float *v,
                          float *out)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *query = NULL;
    UmiAiTrainingTensor *key = NULL;
    UmiAiTrainingTensor *value = NULL;
    UmiAiTrainingTensor *attention = NULL;
    UmiStatus status = UmiAiTrainingTapeCreate(&tape);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingTensorCreate(tape, 4U, 2U, q, 8U, 0, &query);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorCreate(tape, 4U, 2U, k, 8U, 0, &key);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorCreate(tape, 4U, 2U, v, 8U, 0, &value);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingCausalAttention(tape, query, key, value, 2U, &attention);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorValues(attention, out, 8U);
    UmiAiTrainingTapeDestroy(tape);
    return status;
}

int main(void)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *q = NULL;
    UmiAiTrainingTensor *k = NULL;
    UmiAiTrainingTensor *v = NULL;
    UmiAiTrainingTensor *output = NULL;
    const float zeros[8] = {0};
    const float values[8] = {2.0F, 4.0F, 6.0F, 8.0F,
                             10.0F, 12.0F, 14.0F, 16.0F};
    const float changed[8] = {2.0F, 4.0F, 60.0F, 80.0F,
                              10.0F, 12.0F, 140.0F, 160.0F};
    float baseline[8] = {0};
    float mutated[8] = {0};
    VERIFY(Evaluate(zeros, zeros, values, baseline) == UMI_STATUS_OK);
    VERIFY(Close(baseline[0], 2.0F) && Close(baseline[1], 4.0F));
    VERIFY(Close(baseline[2], 4.0F) && Close(baseline[3], 6.0F));
    VERIFY(Close(baseline[4], 10.0F) && Close(baseline[5], 12.0F));
    VERIFY(Close(baseline[6], 12.0F) && Close(baseline[7], 14.0F));
    VERIFY(Evaluate(zeros, zeros, changed, mutated) == UMI_STATUS_OK);
    /* t=0 cannot see the later token, even if it changes by 10x. */
    VERIFY(Close(mutated[0], baseline[0]) && Close(mutated[1], baseline[1]));
    VERIFY(Close(mutated[4], baseline[4]) && Close(mutated[5], baseline[5]));
    VERIFY(!Close(mutated[2], baseline[2]));
    VERIFY(!Close(mutated[6], baseline[6]));

    VERIFY(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingTensorCreate(tape, 4U, 2U, zeros, 8U, 0, &q) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingTensorCreate(tape, 4U, 2U, zeros, 8U, 0, &k) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingTensorCreate(tape, 4U, 2U, values, 8U, 0, &v) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingCausalAttention(tape, q, k, v, 3U, &output)
           == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(output == NULL);
    VERIFY(UmiAiTrainingCausalAttention(tape, q, k, v, 0U, &output)
           == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(output == NULL);
    VERIFY(UmiAiTrainingCausalAttention(tape, q, k, v, 2U, &output)
           == UMI_STATUS_OK);
    UmiAiTrainingTapeDestroy(tape);

    /* A single sequence with unequal logits must use stable softmax. */
    tape = NULL; q = NULL; k = NULL; v = NULL; output = NULL;
    const float qTwo[] = {0.0F, 0.0F, 1.0F, 0.0F};
    const float kTwo[] = {0.0F, 0.0F, 1.5536724F, 0.0F};
    const float vTwo[] = {2.0F, 0.0F, 10.0F, 0.0F};
    float outTwo[4] = {0};
    VERIFY(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingTensorCreate(tape, 2U, 2U, qTwo, 4U, 0, &q) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingTensorCreate(tape, 2U, 2U, kTwo, 4U, 0, &k) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingTensorCreate(tape, 2U, 2U, vTwo, 4U, 0, &v) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingCausalAttention(tape, q, k, v, 2U, &output) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingTensorValues(output, outTwo, 4U) == UMI_STATUS_OK);
    VERIFY(Close(outTwo[0], 2.0F) && Close(outTwo[1], 0.0F));
    /* key1 * query1 / sqrt(2) = ln(3), hence p(key1)=3/4. */
    VERIFY(Close(outTwo[2], 8.0F) && Close(outTwo[3], 0.0F));
    UmiAiTrainingTapeDestroy(tape);
    puts("causal masking and independent sequence verification passed");
    return 0;
}
