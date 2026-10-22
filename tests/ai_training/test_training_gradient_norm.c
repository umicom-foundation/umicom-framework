/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_gradient_norm.c
 * PURPOSE: Validate the bounded numerical contract for global L2 clipping.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/gradient_clipping.h"
#include <float.h>
#include <math.h>
#include <stdio.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

int main(void)
{
    const float first[] = {3.0F, 4.0F};
    const float second[] = {-12.0F, 0.0F};
    const float *arrays[] = {first, second};
    const size_t lengths[] = {2U, 2U};
    UmiAiTrainingGradientClipReport result = {0};
    CHECK(UmiAiTrainingGradientGlobalNorm(arrays, lengths, 2U,
          6.5F, &result) == UMI_STATUS_OK);
    CHECK(fabs(result.globalNorm - 13.0) < 1e-12);
    CHECK(fabs(result.appliedScale - 0.5) < 1e-12);
    CHECK(result.wasClipped == 1 && result.gradientElements == 4U);

    CHECK(UmiAiTrainingGradientGlobalNorm(arrays, lengths, 2U,
          13.0F, &result) == UMI_STATUS_OK);
    CHECK(result.wasClipped == 0 && result.appliedScale == 1.0);
    const float zeros[] = {0.0F, -0.0F};
    const float *empty[] = {zeros};
    const size_t two[] = {2U};
    CHECK(UmiAiTrainingGradientGlobalNorm(empty, two, 1U,
          1e-6F, &result) == UMI_STATUS_OK);
    CHECK(result.globalNorm == 0.0 && result.appliedScale == 1.0);

    /* Neither FP32 squaring nor a naive FP32 sum is permitted: the norm can
     * exceed FLT_MAX even though all stored elements are valid. */
    const float huge[] = {FLT_MAX, FLT_MAX};
    const float *large[] = {huge};
    CHECK(UmiAiTrainingGradientGlobalNorm(large, two, 1U,
          FLT_MAX, &result) == UMI_STATUS_OK);
    CHECK(isfinite(result.globalNorm));
    CHECK(result.globalNorm > (double)FLT_MAX);
    CHECK(fabs(result.appliedScale - 1.0 / sqrt(2.0)) < 1e-12);
    return 0;
}
