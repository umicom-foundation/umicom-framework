/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_rotary_real_tape_gradients.c
 *
 * PURPOSE:
 *   Independently compare the REAL training-tape adjoints for 24 projected
 *   query/key values with central finite differences of the same forward
 *   operator. No decoder, optimiser or surrogate tape is linked by CMake.
 *
 * ARCHITECTURE AND LIMITATIONS:
 *   This is an acceptance regression for the existing bounded rotary tape
 *   composition, NOT a fused operator or a general language model. A finite
 *   difference of the Framework forward path tests its backward path; the
 *   independent full-attention numerical oracle remains the earlier separate
 *   test_training_rotary_qualification_oracle.c regression.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_tape.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define VERIFY(condition) do { if (!(condition)) { \
    fprintf(stderr, "real-tape-gradient line %d: %s\n", __LINE__, #condition); \
    return 1; \
} } while (0)

static const float TOKEN_QUERIES[12] = {
    0.25F, 0.55F, 0.35F, 0.15F,
    0.65F, 0.20F, 0.45F, 0.30F,
    0.40F, 0.75F, 0.15F, 0.60F
};
static const float TOKEN_KEYS[12] = {
    0.50F, 0.15F, 0.45F, 0.25F,
    0.20F, 0.60F, 0.30F, 0.55F,
    0.70F, 0.35F, 0.65F, 0.20F
};
static const float TOKEN_VALUES[12] = {
    0.10F, 0.30F, 0.80F, 0.15F,
    0.60F, 0.20F, 0.10F, 0.45F,
    0.25F, 0.75F, 0.40F, 0.50F
};
static const float TARGETS[12] = {
    0.12F, 0.28F, 0.70F, 0.22F,
    0.35F, 0.35F, 0.40F, 0.30F,
    0.42F, 0.50F, 0.32F, 0.45F
};

typedef struct Calculation {
    float loss;
    float queryGradient[12];
    float keyGradient[12];
    size_t peakBytes;
} Calculation;

/* Every probe creates, differentiates and destroys its own production tape.
 * No output becomes visible until the entire experiment succeeds. */
static UmiStatus Evaluate(const float queryValues[12],
                          const float keyValues[12],
                          uint64_t firstPosition, int calculateGradients,
                          Calculation *outCalculation)
{
    const UmiAiTrainingRotaryConfig rotary = {2U, 2U, 10000.0};
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *query = NULL, *key = NULL, *value = NULL;
    UmiAiTrainingTensor *target = NULL, *attended = NULL, *loss = NULL;
    UmiAiTrainingTapeMemory memory = {0};
    Calculation candidate = {0};
    UmiStatus status;
    if (queryValues == NULL || keyValues == NULL || outCalculation == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingTapeCreate(&tape);
    if (status != UMI_STATUS_OK) return status;
#define EVALUATE_STEP(operation) do { \
    status = (operation); \
    if (status != UMI_STATUS_OK) goto cleanup; \
} while (0)
    EVALUATE_STEP(UmiAiTrainingTensorCreate(tape, 3U, 4U,
        queryValues, 12U, 1, &query));
    EVALUATE_STEP(UmiAiTrainingTensorCreate(tape, 3U, 4U,
        keyValues, 12U, 1, &key));
    EVALUATE_STEP(UmiAiTrainingTensorCreate(tape, 3U, 4U,
        TOKEN_VALUES, 12U, 0, &value));
    EVALUATE_STEP(UmiAiTrainingTensorCreate(tape, 3U, 4U,
        TARGETS, 12U, 0, &target));
    EVALUATE_STEP(UmiAiTrainingRotaryTapeAttention(tape,
        query, key, value, &rotary, 3U, 2U,
        &firstPosition, 1U, &attended));
    EVALUATE_STEP(UmiAiTrainingMeanSquaredError(tape, attended, target, &loss));
    EVALUATE_STEP(UmiAiTrainingTensorValues(loss, &candidate.loss, 1U));
    if (!isfinite(candidate.loss)) {
        status = UMI_STATUS_INVALID_STATE;
        goto cleanup;
    }
    if (calculateGradients) {
        EVALUATE_STEP(UmiAiTrainingBackward(tape, loss));
        EVALUATE_STEP(UmiAiTrainingTensorGradients(query,
            candidate.queryGradient, 12U));
        EVALUATE_STEP(UmiAiTrainingTensorGradients(key,
            candidate.keyGradient, 12U));
    }
    EVALUATE_STEP(UmiAiTrainingTapeMemoryInspect(tape, &memory));
    candidate.peakBytes = memory.peakBytes;
    *outCalculation = candidate;
    status = UMI_STATUS_OK;
#undef EVALUATE_STEP
cleanup:
    UmiAiTrainingTapeDestroy(tape);
    return status;
}

int main(void)
{
    const uint64_t positions[] = {0U, 17U, 513U};
    const double difference = 0.002;
    double largestError = 0.0;
    size_t checked = 0U;
    Calculation origin = {0};
    VERIFY(Evaluate(TOKEN_QUERIES, TOKEN_KEYS, positions[0], 1, &origin)
           == UMI_STATUS_OK);
    VERIFY(origin.peakBytes > 0U);
    for (size_t positionIndex = 0U;
         positionIndex < sizeof positions / sizeof positions[0];
         ++positionIndex) {
        Calculation analytic = {0};
        VERIFY(Evaluate(TOKEN_QUERIES, TOKEN_KEYS,
                        positions[positionIndex], 1, &analytic) == UMI_STATUS_OK);
        /* Uniform position shifts preserve relative attention structure.
         * Floating-point trigonometry is only approximately invariant. */
        VERIFY(fabs((double)analytic.loss - (double)origin.loss) < 0.00003);
        for (size_t operand = 0U; operand < 2U; ++operand) {
            for (size_t index = 0U; index < 12U; ++index) {
                float queryUp[12], queryDown[12], keyUp[12], keyDown[12];
                Calculation higher = {0}, lower = {0};
                memcpy(queryUp, TOKEN_QUERIES, sizeof queryUp);
                memcpy(queryDown, TOKEN_QUERIES, sizeof queryDown);
                memcpy(keyUp, TOKEN_KEYS, sizeof keyUp);
                memcpy(keyDown, TOKEN_KEYS, sizeof keyDown);
                float *up = operand == 0U ? queryUp : keyUp;
                float *down = operand == 0U ? queryDown : keyDown;
                const double baseline = operand == 0U ?
                    (double)TOKEN_QUERIES[index] : (double)TOKEN_KEYS[index];
                up[index] = (float)(baseline + difference);
                down[index] = (float)(baseline - difference);
                VERIFY(Evaluate(queryUp, keyUp, positions[positionIndex],
                                0, &higher) == UMI_STATUS_OK);
                VERIFY(Evaluate(queryDown, keyDown, positions[positionIndex],
                                0, &lower) == UMI_STATUS_OK);
                const double numerator = (double)higher.loss - (double)lower.loss;
                const double denominator = (double)up[index] - (double)down[index];
                const double numerical = numerator / denominator;
                const double tapeGradient = operand == 0U ?
                    (double)analytic.queryGradient[index] :
                    (double)analytic.keyGradient[index];
                const double error = fabs(numerical - tapeGradient);
                if (!isfinite(error) || error > 0.0020) {
                    fprintf(stderr, "position=%llu operand=%zu index=%zu numerical=%.9g autodiff=%.9g abs_error=%.9g\n",
                        (unsigned long long)positions[positionIndex], operand,
                        index, numerical, tapeGradient, error);
                    return 1;
                }
                if (error > largestError) largestError = error;
                ++checked;
            }
        }
    }
    VERIFY(checked == 72U);
    printf("real Framework tape rotary gradients: %zu finite differences, largest absolute error %.9g PASS\n",
           checked, largestError);
    return 0;
}
