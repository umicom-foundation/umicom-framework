/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_rotary_qualification_oracle.c
 *
 * PURPOSE:
 *   Independently qualify one native RoPE-attention training step against
 *   hand-constructed causal scores, stable softmax, MSE and finite differences.
 *
 * SAFETY / SCOPE:
 *   TEST-ONLY numerical reference. No independent neural kernel or gradient
 *   engine is added to Framework production libraries. Under the standard
 *   native-LLM CMake project this executable links the REAL Umicom::AiTraining
 *   implementation, including production tape.c and the RoPE tape bridge.
 *   Checks do not claim that the small experiment is a full language model.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_optimisation.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); \
    return 1; \
} } while (0)

/* Independent numerical oracle for a three-row, two-head, 4-channel task.
 * It deliberately avoids all Framework tape APIs and rotary helpers. Each
 * calculation follows the published mathematical definition directly.
 * The explicit FP32 projection/rotation boundaries reflect the C23 tensor
 * contract; double intermediates reduce oracle round-off error. */
static double ReferenceLoss(const UmiAiTrainingRotaryOptimisationProblem *p)
{
    float q[3][4] = {{0}}, k[3][4] = {{0}};
    float rq[3][4] = {{0}}, rk[3][4] = {{0}};
    double squared = 0.0;
    for (size_t row = 0U; row < 3U; ++row) {
        for (size_t column = 0U; column < 4U; ++column) {
            double query = 0.0, key = 0.0;
            for (size_t inner = 0U; inner < 4U; ++inner) {
                const double x = p->tokenFeatures[4U * row + inner];
                query += x * p->queryWeights[4U * inner + column];
                key += x * p->keyWeights[4U * inner + column];
            }
            q[row][column] = (float)query;
            k[row][column] = (float)key;
        }
        /* Two independent heads, each containing one RoPE coordinate pair.
         * For head width 2 the sole angular frequency is 1, so the base
         * parameter does not affect this deliberately bounded experiment. */
        const double angle = (double)(p->firstTokenPosition + row);
        const double sn = sin(angle), cs = cos(angle);
        for (size_t head = 0U; head < 2U; ++head) {
            const size_t first = 2U * head;
            rq[row][first] = (float)((double)q[row][first] * cs -
                                      (double)q[row][first + 1U] * sn);
            rq[row][first + 1U] = (float)((double)q[row][first] * sn +
                                           (double)q[row][first + 1U] * cs);
            rk[row][first] = (float)((double)k[row][first] * cs -
                                      (double)k[row][first + 1U] * sn);
            rk[row][first + 1U] = (float)((double)k[row][first] * sn +
                                           (double)k[row][first + 1U] * cs);
        }
    }
    for (size_t row = 0U; row < 3U; ++row) {
        for (size_t head = 0U; head < 2U; ++head) {
            const size_t first = 2U * head;
            double scores[3] = {0}, weights[3] = {0};
            double maximum = -INFINITY, denominator = 0.0;
            /* Causal attention: a row sees only itself and earlier rows. */
            for (size_t past = 0U; past <= row; ++past) {
                double dot = 0.0;
                for (size_t channel = 0U; channel < 2U; ++channel)
                    dot += (double)rq[row][first + channel] *
                           (double)rk[past][first + channel];
                scores[past] = dot / sqrt(2.0);
                if (scores[past] > maximum) maximum = scores[past];
            }
            for (size_t past = 0U; past <= row; ++past) {
                weights[past] = exp(scores[past] - maximum);
                denominator += weights[past];
            }
            for (size_t channel = 0U; channel < 2U; ++channel) {
                double attended = 0.0;
                for (size_t past = 0U; past <= row; ++past)
                    attended += weights[past] / denominator *
                        (double)p->values[4U * past + first + channel];
                /* Framework MSE consumes an FP32 attention-tensor result. */
                const double difference = (double)(float)attended -
                    (double)p->desiredAttention[4U * row + first + channel];
                squared += difference * difference / 12.0;
            }
        }
    }
    return squared;
}

/* Fixed inputs are interpretable, finite software-training features, not
 * financial ledger amounts. The reference deliberately contains no random
 * number generator or hidden initialisation. */
static UmiAiTrainingRotaryOptimisationProblem Example(void)
{
    return (UmiAiTrainingRotaryOptimisationProblem){
        .tokenFeatures = {.25F,.50F,.75F,.20F, .40F,.10F,.30F,.80F,
                          .90F,.70F,.20F,.10F},
        .values = {.10F,.40F,.70F,.20F, .20F,.10F,.80F,.60F,
                   .90F,.20F,.10F,.50F},
        .desiredAttention = {.10F,.40F,.70F,.20F, .50F,.25F,.50F,.50F,
                             .65F,.35F,.33F,.50F},
        .queryWeights = {1.F,.1F,0.F,0.F, 0.F,1.F,.1F,0.F,
                         0.F,0.F,1.F,.1F, .1F,0.F,0.F,1.F},
        .keyWeights = {1.F,0.F,.1F,0.F, 0.F,1.F,0.F,.1F,
                       .1F,0.F,1.F,0.F, 0.F,.1F,0.F,1.F},
        .firstTokenPosition = 17U, .frequencyBase = 10000.0
    };
}

int main(void)
{
    const UmiAiTrainingRotaryOptimisationProblem baseline = Example();
    const UmiAiTrainingRotaryOptimisationPlan plan = {1U, 0.5F};
    UmiAiTrainingRotaryOptimisationResult measured = {0};
    UmiAiTrainingRotaryOptimisationProblem adjusted = baseline;
    double largestGradientDifference = 0.0;
    const double delta = 0.002;
    CHECK(UmiAiTrainingRotaryOptimisationRun(&baseline, &plan, &measured)
          == UMI_STATUS_OK);
    CHECK(measured.completedSteps == 1U && measured.peakTapeBytes > 0U);
    CHECK(isfinite(measured.initialLoss) && isfinite(measured.finalLoss));
    CHECK(fabs((double)measured.initialLoss - ReferenceLoss(&baseline)) < 0.000015);
    /* Obtain the real post-update weight matrix and verify its fresh loss. */
    memcpy(adjusted.queryWeights, measured.finalQueryWeights,
           sizeof adjusted.queryWeights);
    memcpy(adjusted.keyWeights, measured.finalKeyWeights,
           sizeof adjusted.keyWeights);
    CHECK(fabs((double)measured.finalLoss - ReferenceLoss(&adjusted)) < 0.000015);
    CHECK(fabs(measured.lossAfterStep[0] - measured.finalLoss) < 0.0000001);

    /* The observed single-step weight change must agree with the central
     * finite-difference gradient of the independent C23 loss oracle.
     * g ≈ (loss(w+delta)-loss(w-delta))/(2*delta).
     * For SGD: g_observed ≈ (w_before-w_after)/learningRate.
     * Test every one of the 32 query/key parameters, including small slopes. */
    for (size_t which = 0U; which < 2U; ++which) {
        for (size_t index = 0U; index < 16U; ++index) {
            const float *trial = which == 0U ? baseline.queryWeights : baseline.keyWeights;
            float *candidate = which == 0U ? adjusted.queryWeights : adjusted.keyWeights;
            const double previously = trial[index];
            UmiAiTrainingRotaryOptimisationProblem plus = baseline, minus = baseline;
            float *plusWeights = which == 0U ? plus.queryWeights : plus.keyWeights;
            float *minusWeights = which == 0U ? minus.queryWeights : minus.keyWeights;
            plusWeights[index] = (float)(previously + delta);
            minusWeights[index] = (float)(previously - delta);
            const double estimated = (ReferenceLoss(&plus) - ReferenceLoss(&minus)) /
                ((double)plusWeights[index] - (double)minusWeights[index]);
            const double observed = (previously - (double)candidate[index]) /
                (double)plan.learningRate;
            const double difference = fabs(estimated - observed);
            if (difference > largestGradientDifference)
                largestGradientDifference = difference;
            if (difference >= 0.0008) {
                fprintf(stderr,
                    "gradient mismatch: matrix=%zu element=%zu independent=%.9f observed=%.9f difference=%.9f\n",
                    which, index, estimated, observed, difference);
                return 1;
            }
        }
    }
    UmiAiTrainingRotaryOptimisationProblem preserved = Example();
    CHECK(memcmp(&baseline, &preserved, sizeof baseline) == 0);
    printf("independent native RoPE audit: initial=%.8f final=%.8f largest-gradient-residual=%.8g (32 checked) PASS\n",
           measured.initialLoss, measured.finalLoss, largestGradientDifference);
    return 0;
}
