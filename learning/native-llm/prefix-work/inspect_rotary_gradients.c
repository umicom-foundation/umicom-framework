/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM Learning Laboratory
 * File: learning/native-llm/prefix-work/inspect_rotary_gradients.c
 * PURPOSE: Observe real C23 RoPE forward and reverse derivatives without a
 *          replacement decoder, script, trainer or checkpoint conversion.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_training.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>

int main(int argc, char **argv)
{
    const UmiAiTrainingRotaryConfig config = {4U, 4U, 10000.0};
    const float query[4] = {0.4F, -0.3F, 0.2F, 0.5F};
    const float key[4] = {-0.1F, 0.4F, 0.8F, -0.2F};
    const double upstream = 1.0;
    const double epsilon = 0.001;
    float gradQ[4] = {0}, gradK[4] = {0};
    double dot = 0.0, largestDifference = 0.0;
    UmiStatus status;
    (void)argv;
    if (argc != 1) {
        fputs("usage: umicom-native-llm-rotary-gradients\n", stderr);
        return 2;
    }
    status = UmiAiTrainingRotaryRelativeDot(&config, query, key, 4U,
                UINT64_C(21), UINT64_C(18), &dot);
    if (status != UMI_STATUS_OK) return 1;
    status = UmiAiTrainingRotaryDotBackward(&config, query, key, 4U,
                UINT64_C(21), UINT64_C(18), upstream, gradQ, 4U, gradK, 4U);
    if (status != UMI_STATUS_OK) return 1;

    /* Observe an independent central difference in an educational client.
     * All production RoPE forward/adjoint maths stay in Umicom Framework. */
    for (size_t channel = 0U; channel < 4U; ++channel) {
        float movedPlus[4] = {0};
        float movedMinus[4] = {0};
        double positive = 0.0, negative = 0.0;
        for (size_t j = 0U; j < 4U; ++j) {
            movedPlus[j] = query[j];
            movedMinus[j] = query[j];
        }
        movedPlus[channel] += (float)epsilon;
        movedMinus[channel] -= (float)epsilon;
        if (UmiAiTrainingRotaryRelativeDot(&config, movedPlus, key, 4U,
                UINT64_C(21), UINT64_C(18), &positive) != UMI_STATUS_OK ||
            UmiAiTrainingRotaryRelativeDot(&config, movedMinus, key, 4U,
                UINT64_C(21), UINT64_C(18), &negative) != UMI_STATUS_OK)
            return 1;
        const double numerical = (positive - negative) / (2.0 * epsilon);
        const double difference = fabs(numerical - (double)gradQ[channel]);
        if (difference > largestDifference) largestDifference = difference;
    }
    printf("Umicom C23 rotary gradient qualification\n"
           "Unscaled rotary Q/K dot: %.7f\n"
           "Absolute token positions: query=21 key=18\n"
           "Independent central difference maximum error: %.8f\n",
           dot, largestDifference);
    if (largestDifference > 0.001) {
        fputs("Rotary derivative mismatch: refusing success\n", stderr);
        return 1;
    }
    puts("Rotary gradients qualified for this bounded example.\n"
         "This is NOT a trained rotary language model or a speed benchmark.\n"
         "Legacy decoder/checkpoint contracts remain unchanged.");
    return 0;
}
