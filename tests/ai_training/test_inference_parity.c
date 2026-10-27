/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_inference_parity.c
 *
 * PURPOSE:
 *   Exercise finite-vector parity, tolerance boundaries, diagnostics and
 *   failure-atomic rejection with the actual native C23 implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/inference_parity.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define REQUIRE(expr) do { if (!(expr)) { \
    fprintf(stderr, "parity test %d: %s\n", __LINE__, #expr); \
    return 1; } } while (0)

int main(void)
{
    const float expected[4] = {0.0F, 1.0F, -2.0F, 1000.0F};
    const float actual[4] = {0.0F, 1.00005F, -2.00010F, 1000.05F};
    const UmiAiTrainingInferenceParityTolerance strict = {0.0F, 0.0F};
    const UmiAiTrainingInferenceParityTolerance loose = {0.001F, 0.0001F};
    UmiAiTrainingInferenceParityReport result = {0};
    REQUIRE(UmiAiTrainingInferenceParityCompare(expected, actual, 4U,
        &strict, &result) == UMI_STATUS_OK);
    REQUIRE(result.valuesCompared == 4U);
    REQUIRE(result.mismatchedValues == 3U);
    REQUIRE(result.firstMismatchIndex == 1U);
    REQUIRE(result.withinTolerance == 0);
    REQUIRE(result.maximumAbsoluteDifference > 0.04);
    REQUIRE(UmiAiTrainingInferenceParityCompare(expected, actual, 4U,
        &loose, &result) == UMI_STATUS_OK);
    REQUIRE(result.withinTolerance == 1 && result.mismatchedValues == 0U);
    REQUIRE(result.firstMismatchIndex == SIZE_MAX);
    UmiAiTrainingInferenceParityReport original = result;
    float nonfinite[4] = {0.0F, 1.0F, NAN, 3.0F};
    REQUIRE(UmiAiTrainingInferenceParityCompare(expected, nonfinite, 4U,
        &loose, &result) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(memcmp(&original, &result, sizeof(original)) == 0);
    const UmiAiTrainingInferenceParityTolerance invalid = {-1.0F, 0.0F};
    REQUIRE(UmiAiTrainingInferenceParityCompare(expected, actual, 4U,
        &invalid, &result) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(memcmp(&original, &result, sizeof(original)) == 0);
    REQUIRE(UmiAiTrainingInferenceParityCompare(expected, actual, 0U,
        &loose, &result) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(memcmp(&original, &result, sizeof(original)) == 0);
    REQUIRE(UmiAiTrainingInferenceParityCompare(expected, actual, 4U,
        &loose, (UmiAiTrainingInferenceParityReport *)(void *)actual)
        == UMI_STATUS_INVALID_ARGUMENT);
    puts("Inference parity: finite vectors, tolerance and atomic rejects pass");
    return 0;
}
