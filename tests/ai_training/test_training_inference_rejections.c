/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_inference_rejections.c
 * PURPOSE: Reject malformed profiling requests without modifying results.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/inference_profile.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
    return 1; } } while (0)

int main(void)
{
    UmiAiTrainingDecoderConfig config = {17U, 6U, 8U, 16U, 2U, UINT64_C(42)};
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingInferenceProfile actual;
    UmiAiTrainingInferenceProfile expected;
    const size_t good[] = {4U, 5U, 6U, 7U};
    const size_t bad[] = {4U, 5U, 17U, 7U};
    memset(&actual, 0xA5, sizeof(actual));
    memcpy(&expected, &actual, sizeof(expected));
    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingInferenceProfileStream(NULL, good, 4U, 2U,
                                                &actual) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingInferenceProfileStream(model, NULL, 4U, 2U,
                                                &actual) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingInferenceProfileStream(model, good, 0U, 0U,
                                                &actual) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingInferenceProfileStream(model, good, 4U, 0U,
                                                &actual) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingInferenceProfileStream(model, good, 4U, 5U,
                                                &actual) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingInferenceProfileStream(model, bad, 4U, 2U,
                                                &actual) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingInferenceProfileStream(model, good, 4U, 2U,
                                                NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&actual, &expected, sizeof(actual)) == 0);
    CHECK(UmiAiTrainingInferenceProfileStream(model, good, 1U, 1U,
                                                &actual) == UMI_STATUS_OK);
    CHECK(actual.referenceQueries == 1U);
    CHECK(actual.referenceComputedRows == 6U);
    CHECK(actual.cacheComputedRows == 1U);
    CHECK(actual.cacheWindowRebuilds == 0U);
    CHECK(actual.retainedTokens == 1U);
    UmiAiTrainingDecoderDestroy(model);
    puts("inference profiler rejected invalid inputs without partial reports: PASS");
    return 0;
}
