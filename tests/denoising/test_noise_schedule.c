/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/denoising/test_noise_schedule.c
 *
 * PURPOSE:
 *   Check cumulative coefficients, endpoint indexing, precision limits and unchanged output on rejected schedules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/denoising.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    float output[5] = {11.0F, 12.0F, 13.0F, 14.0F, 15.0F};
    float before[5];
    memcpy(before, output, sizeof(output));
    const float betas[3] = {0.1F, 0.2F, 0.3F};
    if (strcmp(argv[1], "reference") == 0) {
        const float expected[3] = {0.9F, 0.72F, 0.504F};
        CHECK(UmiAiDenoisingScheduleFromBetas(betas, 3U, output, 5U) == UMI_STATUS_OK);
        for (size_t i = 0U; i < 3U; ++i) CHECK(fabsf(output[i] - expected[i]) < 0.000001F);
        CHECK(output[3] == before[3] && output[4] == before[4]);
    } else if (strcmp(argv[1], "zero-and-single") == 0) {
        const float zero[3] = {0.0F, -0.0F, 0.0F};
        CHECK(UmiAiDenoisingScheduleFromBetas(zero, 3U, output, 5U) == UMI_STATUS_OK);
        CHECK(output[0] == 1.0F && output[1] == 1.0F && output[2] == 1.0F);
        const float one[1] = {0.25F};
        CHECK(UmiAiDenoisingScheduleFromBetas(one, 1U, output, 5U) == UMI_STATUS_OK);
        CHECK(output[0] == 0.75F && output[1] == 1.0F && output[4] == 15.0F);
    } else if (strcmp(argv[1], "refusals") == 0) {
        const float invalid[] = {-0.01F, 1.0F, 1.01F, NAN, INFINITY, -INFINITY};
        for (size_t bad = 0U; bad < sizeof(invalid) / sizeof(invalid[0]); ++bad) {
            /* A valid prefix must not become visible before a later rejection. */
            const float values[3] = {0.1F, 0.2F, invalid[bad]};
            CHECK(UmiAiDenoisingScheduleFromBetas(values, 3U, output, 5U) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(memcmp(output, before, sizeof(output)) == 0);
        }
        CHECK(UmiAiDenoisingScheduleFromBetas(betas, 3U, output, 2U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(output, before, sizeof(output)) == 0);
    } else if (strcmp(argv[1], "arguments") == 0) {
        CHECK(UmiAiDenoisingScheduleFromBetas(NULL, 3U, output, 5U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiDenoisingScheduleFromBetas(betas, 3U, NULL, 5U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiDenoisingScheduleFromBetas(betas, 0U, output, 5U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiAiDenoisingScheduleFromBetas(betas, UMI_AI_DENOISING_MAX_STEPS + 1U, output, 5U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(output, before, sizeof(output)) == 0);
    } else if (strcmp(argv[1], "precision-boundary") == 0) {
        /* The Framework training target uses binary floats. Derive the last
         * normal power-of-two step from its limits instead of assuming 126. */
        CHECK(FLT_RADIX == 2 && FLT_MIN_EXP < 0);
        const size_t lastNormal = (size_t)(1 - FLT_MIN_EXP);
        CHECK(lastNormal + 1U < UMI_AI_DENOISING_MAX_STEPS);
        float halves[UMI_AI_DENOISING_MAX_STEPS], values[UMI_AI_DENOISING_MAX_STEPS];
        for (size_t i = 0U; i < UMI_AI_DENOISING_MAX_STEPS; ++i) { halves[i] = 0.5F; values[i] = 17.0F; }
        CHECK(UmiAiDenoisingScheduleFromBetas(halves, lastNormal + 1U, values,
            UMI_AI_DENOISING_MAX_STEPS) == UMI_STATUS_INVALID_ARGUMENT);
        for (size_t i = 0U; i < UMI_AI_DENOISING_MAX_STEPS; ++i) CHECK(values[i] == 17.0F);
        CHECK(UmiAiDenoisingScheduleFromBetas(halves, lastNormal, values,
            UMI_AI_DENOISING_MAX_STEPS) == UMI_STATUS_OK);
        CHECK(values[lastNormal - 1U] == FLT_MIN && values[lastNormal] == 17.0F);
    } else if (strcmp(argv[1], "maximum") == 0) {
        float values[UMI_AI_DENOISING_MAX_STEPS], result[UMI_AI_DENOISING_MAX_STEPS];
        for (size_t i = 0U; i < UMI_AI_DENOISING_MAX_STEPS; ++i) values[i] = 0.001F;
        CHECK(UmiAiDenoisingScheduleFromBetas(values, UMI_AI_DENOISING_MAX_STEPS,
            result, UMI_AI_DENOISING_MAX_STEPS) == UMI_STATUS_OK);
        long double reference = 1.0L;
        for (size_t i = 0U; i < UMI_AI_DENOISING_MAX_STEPS; ++i) {
            reference *= 1.0L - (long double)values[i];
            CHECK(fabsl((long double)result[i] - reference) / reference < 0.000001L);
            CHECK(result[i] > 0.0F && result[i] <= (i == 0U ? 1.0F : result[i - 1U]));
        }
    } else if (strcmp(argv[1], "mix-recover") == 0) {
        CHECK(UmiAiDenoisingScheduleFromBetas(betas, 3U, output, 5U) == UMI_STATUS_OK);
        const float clean[2] = {0.2F, -0.4F}, noise[2] = {0.6F, -0.8F};
        for (size_t step = 0U; step < 3U; ++step) {
            float noisy[2], recovered[2];
            CHECK(UmiAiDenoisingMix(clean, noise, 2U, output[step], noisy, 2U) == UMI_STATUS_OK);
            CHECK(UmiAiDenoisingRecover(noisy, noise, 2U, output[step], recovered, 2U) == UMI_STATUS_OK);
            for (size_t i = 0U; i < 2U; ++i) CHECK(fabsf(recovered[i] - clean[i]) < 0.000001F);
        }
    } else return 2;
    return 0;
}
