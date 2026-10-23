/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_gradient_clipping_rejections.c
 * PURPOSE: Reject invalid gradients and preserve caller-owned diagnostics.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_gradient_clipping_rejections.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/gradient_clipping.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

int main(void)
{
    const float good[] = {3.0F, 4.0F};
    const float bad[] = {1.0F, NAN};
    const float *arrays[] = {good, bad};
    const size_t counts[] = {2U, 2U};
    const size_t empty[] = {0U, 2U};
    UmiAiTrainingGradientClipReport report = {9876.0, 0.75, 99U, 1};
    const UmiAiTrainingGradientClipReport preserved = report;
    CHECK(UmiAiTrainingGradientGlobalNorm(arrays, counts, 2U,
          1.0F, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report, &preserved, sizeof(report)) == 0);
    arrays[1] = good;
    CHECK(UmiAiTrainingGradientGlobalNorm(arrays, empty, 2U,
          1.0F, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingGradientGlobalNorm(arrays, counts, 2U,
          NAN, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingGradientGlobalNorm(arrays, counts, 2U,
          0.0F, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingGradientGlobalNorm(arrays, counts, 2U,
          -1.0F, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingGradientGlobalNorm(arrays, counts, 0U,
          1.0F, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingGradientGlobalNorm(arrays, counts,
          UMI_AI_TRAINING_MAX_TENSORS + 1U,
          1.0F, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report, &preserved, sizeof(report)) == 0);
    CHECK(good[0] == 3.0F && good[1] == 4.0F);
    return 0;
}
