/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_embedding.c
 * PURPOSE: Verify embedding gather/scatter, repeated tokens and rejection.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/tape.h"

#include <math.h>
#include <stdio.h>

#define VERIFY(ok) do { if (!(ok)) { \
    fprintf(stderr, "embedding verification failed, line %d: %s\n", \
        __LINE__, #ok); return 1; } } while (0)

static int Close(float left, float right)
{
    return fabsf(left - right) < 0.0001F;
}

int main(void)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *table = NULL;
    UmiAiTrainingTensor *output = NULL;
    UmiAiTrainingTensor *zeroTarget = NULL;
    UmiAiTrainingTensor *loss = NULL;
    UmiAiTrainingTensor *unmodified = NULL;
    const float tableValues[] = {1.0F, 2.0F, 3.0F, 4.0F, 5.0F, 6.0F};
    const float zeros[] = {0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F};
    const size_t ids[] = {2U, 0U, 2U};
    const size_t invalidIds[] = {2U, 100U};
    float outputValues[6] = {0};
    float gradients[6] = {0};
    size_t rows = 0U;
    size_t columns = 0U;

    VERIFY(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingTensorCreate(tape, 3U, 2U, tableValues, 6U, 1, &table)
           == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingEmbeddingLookup(tape, table, invalidIds, 2U, &unmodified)
           == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(unmodified == NULL);
    VERIFY(UmiAiTrainingEmbeddingLookup(tape, table, ids, 3U, &output)
           == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingTensorShape(output, &rows, &columns) == UMI_STATUS_OK);
    VERIFY(rows == 3U && columns == 2U);
    VERIFY(UmiAiTrainingTensorValues(output, outputValues, 6U) == UMI_STATUS_OK);
    VERIFY(Close(outputValues[0], 5.0F) && Close(outputValues[1], 6.0F));
    VERIFY(Close(outputValues[2], 1.0F) && Close(outputValues[3], 2.0F));
    VERIFY(Close(outputValues[4], 5.0F) && Close(outputValues[5], 6.0F));
    VERIFY(UmiAiTrainingTensorCreate(tape, 3U, 2U, zeros, 6U, 0, &zeroTarget)
           == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingMeanSquaredError(tape, output, zeroTarget, &loss)
           == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingBackward(tape, loss) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingTensorGradients(table, gradients, 6U) == UMI_STATUS_OK);
    VERIFY(Close(gradients[0], 1.0F / 3.0F));
    VERIFY(Close(gradients[1], 2.0F / 3.0F));
    VERIFY(Close(gradients[2], 0.0F) && Close(gradients[3], 0.0F));
    VERIFY(Close(gradients[4], 10.0F / 3.0F));
    VERIFY(Close(gradients[5], 4.0F));
    VERIFY(UmiAiTrainingSgdStep(tape, table, 0.1F) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingTapeRewind(tape, 1U) == UMI_STATUS_OK);
    UmiAiTrainingTapeDestroy(tape);
    puts("embedding forward/backward verification passed");
    return 0;
}
