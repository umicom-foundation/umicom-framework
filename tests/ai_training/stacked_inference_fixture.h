/*-----------------------------------------------------------------------------
 * Umicom Framework | Native multi-layer inference test data
 * PURPOSE: Share real Framework decoder setup and simple synthetic examples.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STACKED_INFERENCE_TEST_FIXTURE_H
#define UMICOM_STACKED_INFERENCE_TEST_FIXTURE_H

#include "umicom/ai/training/stacked_inference.h"
#include "umicom/ai/training/stacked_evaluation.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "stacked inference line %d: %s\n", __LINE__, #x); \
    return 1; \
} } while (0)

static inline UmiStatus MakeModel(uint64_t seed,
                                 UmiAiTrainingStackedDecoder **out)
{
    const UmiAiTrainingStackedDecoderConfig config = {
        260U, 4U, 8U, 16U, 2U, 2U, seed
    };
    return UmiAiTrainingStackedDecoderCreate(&config, out);
}

static inline UmiAiTrainingStackedEvaluationCase Example(
    const size_t *inputs, const size_t *targets, const uint8_t *mask)
{
    UmiAiTrainingStackedEvaluationCase result = {0};
    result.inputIds = inputs;
    result.targetIds = targets;
    result.lossMask = mask;
    result.tokenCount = 4U;
    return result;
}

#endif
