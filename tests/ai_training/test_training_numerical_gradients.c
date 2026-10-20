/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_numerical_gradients.c
 * PURPOSE:
 *   Compare the chain-rule derivative of Matmul + CrossEntropy with central
 *   finite differences. The numerical reference is independently computed by
 *   running perturbed forward passes, not by calling the gradient routine.
 * AUTHOR: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/tape.h"
#include <math.h>
#include <stdio.h>

#define REQUIRE(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "numerical gradient test failed at line %d: %s\n", \
                __LINE__, #condition); \
        return 1; \
    } \
} while (0)

/* Common evaluation graph: two examples, two features and three classes. */
static UmiStatus Evaluate(const float weights[6],
                          float *outLoss, float outGradient[6], int backward)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *features = NULL;
    UmiAiTrainingTensor *parameters = NULL;
    UmiAiTrainingTensor *logits = NULL;
    UmiAiTrainingTensor *loss = NULL;
    static const float x[4] = {1.0F, -0.5F, 0.25F, 2.0F};
    static const size_t labels[2] = {2U, 1U};
    UmiStatus result = UmiAiTrainingTapeCreate(&tape);
    if (result != UMI_STATUS_OK) return result;
#define EVALUATE(operation) do { \
    result = (operation); \
    if (result != UMI_STATUS_OK) goto done; \
} while (0)
    EVALUATE(UmiAiTrainingTensorCreate(tape, 2U, 2U, x, 4U, 0, &features));
    EVALUATE(UmiAiTrainingTensorCreate(tape, 2U, 3U, weights, 6U, 1, &parameters));
    EVALUATE(UmiAiTrainingMatmul(tape, features, parameters, &logits));
    EVALUATE(UmiAiTrainingCrossEntropy(tape, logits, labels, 2U, &loss));
    EVALUATE(UmiAiTrainingTensorValues(loss, outLoss, 1U));
    if (backward) {
        EVALUATE(UmiAiTrainingBackward(tape, loss));
        EVALUATE(UmiAiTrainingTensorGradients(parameters, outGradient, 6U));
    }
done:
    UmiAiTrainingTapeDestroy(tape);
    return result;
#undef EVALUATE
}

int main(void)
{
    static const float initial[6] = {0.2F, -0.1F, 0.3F, -0.4F, 0.5F, -0.2F};
    float w[6];
    float loss = 0.0F;
    float grad[6] = {0.0F};
    const float epsilon = 0.001F;
    REQUIRE(Evaluate(initial, &loss, grad, 1) == UMI_STATUS_OK);
    REQUIRE(loss > 0.0F);
    for (size_t i = 0U; i < 6U; ++i) {
        float plus = 0.0F;
        float minus = 0.0F;
        float numerical;
        for (size_t j = 0U; j < 6U; ++j) w[j] = initial[j];
        w[i] += epsilon;
        REQUIRE(Evaluate(w, &plus, NULL, 0) == UMI_STATUS_OK);
        w[i] -= 2.0F * epsilon;
        REQUIRE(Evaluate(w, &minus, NULL, 0) == UMI_STATUS_OK);
        numerical = (plus - minus) / (2.0F * epsilon);
        REQUIRE(fabsf(numerical - grad[i]) < 0.00025F);
    }
    puts("central finite-difference gradients agree with autodiff");
    return 0;
}
