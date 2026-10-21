/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_attention_gradients.c
 * PURPOSE: Verify attention Q, K, V derivatives against numerical gradients.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/tape.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define VERIFY(ok) do { if (!(ok)) { \
    fprintf(stderr, "attention derivative verification failed, line %d: %s\n", \
        __LINE__, #ok); return 1; } } while (0)

/* Returns the independent forward loss for a given group of Q/K/V values. */
static UmiStatus EvaluateLoss(const float q[4], const float k[4],
                             const float v[4], float *outLoss)
{
    const float zeros[4] = {0};
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *query = NULL;
    UmiAiTrainingTensor *key = NULL;
    UmiAiTrainingTensor *value = NULL;
    UmiAiTrainingTensor *target = NULL;
    UmiAiTrainingTensor *attention = NULL;
    UmiAiTrainingTensor *loss = NULL;
    UmiStatus status = UmiAiTrainingTapeCreate(&tape);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingTensorCreate(tape, 2U, 2U, q, 4U, 0, &query);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorCreate(tape, 2U, 2U, k, 4U, 0, &key);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorCreate(tape, 2U, 2U, v, 4U, 0, &value);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorCreate(tape, 2U, 2U, zeros, 4U, 0, &target);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingCausalAttention(tape, query, key, value, 2U, &attention);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingMeanSquaredError(tape, attention, target, &loss);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorValues(loss, outLoss, 1U);
    UmiAiTrainingTapeDestroy(tape);
    return status;
}

/* When Q, K and V are the same tape-owned tensor, the three derivative
 * paths must accumulate into one parameter array instead of overwriting it. */
static UmiStatus EvaluateAliasedLoss(const float input[4], float *outLoss)
{
    const float zeros[4] = {0};
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *shared = NULL;
    UmiAiTrainingTensor *target = NULL;
    UmiAiTrainingTensor *attention = NULL;
    UmiAiTrainingTensor *loss = NULL;
    UmiStatus status = UmiAiTrainingTapeCreate(&tape);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingTensorCreate(tape, 2U, 2U, input, 4U, 0, &shared);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorCreate(tape, 2U, 2U, zeros, 4U, 0, &target);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingCausalAttention(tape, shared, shared, shared, 2U,
                                               &attention);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingMeanSquaredError(tape, attention, target, &loss);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorValues(loss, outLoss, 1U);
    UmiAiTrainingTapeDestroy(tape);
    return status;
}

int main(void)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *nodes[3] = {NULL, NULL, NULL};
    UmiAiTrainingTensor *target = NULL;
    UmiAiTrainingTensor *output = NULL;
    UmiAiTrainingTensor *loss = NULL;
    float params[3][4] = {
        {0.20F, -0.35F, 0.6F, 0.15F},
        {0.50F, 0.25F, -0.4F, 0.8F},
        {0.70F, -0.20F, 0.1F, 0.9F}
    };
    const float zeros[4] = {0};
    float gradients[3][4] = {{0}};
    const float epsilon = 0.001F;
    VERIFY(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    for (size_t n = 0U; n < 3U; ++n) {
        VERIFY(UmiAiTrainingTensorCreate(tape, 2U, 2U, params[n], 4U, 1, &nodes[n])
               == UMI_STATUS_OK);
    }
    VERIFY(UmiAiTrainingTensorCreate(tape, 2U, 2U, zeros, 4U, 0, &target)
           == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingCausalAttention(tape, nodes[0], nodes[1], nodes[2],
                                        2U, &output) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingMeanSquaredError(tape, output, target, &loss)
           == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingBackward(tape, loss) == UMI_STATUS_OK);
    for (size_t n = 0U; n < 3U; ++n) {
        VERIFY(UmiAiTrainingTensorGradients(nodes[n], gradients[n], 4U) == UMI_STATUS_OK);
    }
    UmiAiTrainingTapeDestroy(tape);

    for (size_t parameter = 0U; parameter < 3U; ++parameter) {
        for (size_t index = 0U; index < 4U; ++index) {
            float plus;
            float minus;
            float original = params[parameter][index];
            params[parameter][index] = original + epsilon;
            VERIFY(EvaluateLoss(params[0], params[1], params[2], &plus) == UMI_STATUS_OK);
            params[parameter][index] = original - epsilon;
            VERIFY(EvaluateLoss(params[0], params[1], params[2], &minus) == UMI_STATUS_OK);
            params[parameter][index] = original;
            float estimate = (plus - minus) / (2.0F * epsilon);
            VERIFY(isfinite(gradients[parameter][index]));
            if (fabsf(estimate - gradients[parameter][index]) >= 0.004F) {
                fprintf(stderr, "component %zu[%zu]: numeric %f, analytic %f\n",
                        parameter, index, (double)estimate,
                        (double)gradients[parameter][index]);
                return 1;
            }
        }
    }
    /* Verify gradients for the alias case against a separate finite-difference
     * oracle. Changing one value must influence all three logical operands. */
    float sharedValues[4] = {0.2F, -0.3F, 0.6F, 0.5F};
    float sharedGradients[4] = {0};
    UmiAiTrainingTensor *shared = NULL;
    UmiAiTrainingTensor *zeroTarget = NULL;
    UmiAiTrainingTensor *aliasedAttention = NULL;
    UmiAiTrainingTensor *aliasedLoss = NULL;
    tape = NULL;
    VERIFY(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingTensorCreate(tape, 2U, 2U, sharedValues, 4U, 1,
                                     &shared) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingTensorCreate(tape, 2U, 2U, zeros, 4U, 0,
                                     &zeroTarget) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingCausalAttention(tape, shared, shared, shared, 2U,
                                        &aliasedAttention) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingMeanSquaredError(tape, aliasedAttention, zeroTarget,
                                         &aliasedLoss) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingBackward(tape, aliasedLoss) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingTensorGradients(shared, sharedGradients, 4U) == UMI_STATUS_OK);
    UmiAiTrainingTapeDestroy(tape);
    for (size_t index = 0U; index < 4U; ++index) {
        float original = sharedValues[index];
        float plus;
        float minus;
        sharedValues[index] = original + epsilon;
        VERIFY(EvaluateAliasedLoss(sharedValues, &plus) == UMI_STATUS_OK);
        sharedValues[index] = original - epsilon;
        VERIFY(EvaluateAliasedLoss(sharedValues, &minus) == UMI_STATUS_OK);
        sharedValues[index] = original;
        VERIFY(fabsf((plus - minus) / (2.0F * epsilon) - sharedGradients[index])
               < 0.004F);
    }
    puts("causal attention finite-difference + aliased-operand verification passed");
    return 0;
}
