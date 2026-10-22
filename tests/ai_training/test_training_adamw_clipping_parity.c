/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_adamw_clipping_parity.c
 * PURPOSE: Compare clipped AdamW with explicitly scaled supplied gradients.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/optimizer.h"
#include "umicom/ai/training/gradient_clipping.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

int main(void)
{
    const float initial[] = {0.5F, -0.25F};
    const float given[] = {3.0F, 4.0F};
    const float scaled[] = {1.5F, 2.0F};
    const float *original[] = {given};
    const float *reference[] = {scaled};
    const UmiAiTrainingAdamWConfig settings = {0.0F, 0.0F, 1.0F, 0.0F};
    UmiAiTrainingTape *tapes[2] = {NULL, NULL};
    UmiAiTrainingTensor *params[2] = {NULL, NULL};
    UmiAiTrainingAdamW *optimizers[2] = {NULL, NULL};
    UmiAiTrainingGradientClipReport report = {0};
    float weights[2][2];
    uint64_t steps[2] = {0U, 0U};
    size_t bytes[2] = {0U, 0U};

    for (size_t i = 0U; i < 2U; ++i) {
        CHECK(UmiAiTrainingTapeCreate(&tapes[i]) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingTensorCreate(tapes[i], 1U, 2U, initial, 2U,
              1, &params[i]) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingAdamWCreate(tapes[i], &params[i], 1U,
              &settings, &optimizers[i]) == UMI_STATUS_OK);
    }
    CHECK(UmiAiTrainingAdamWStepWithClippedGradients(
          optimizers[0], original, 0.1F, 2.5F, &report) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWStepWithGradients(
          optimizers[1], reference, 0.1F) == UMI_STATUS_OK);
    CHECK(report.gradientElements == 2U && report.wasClipped == 1);
    CHECK(fabs(report.globalNorm - 5.0) < 1e-12);
    CHECK(fabs(report.appliedScale - 0.5) < 1e-12);
    for (size_t i = 0U; i < 2U; ++i) {
        CHECK(UmiAiTrainingTensorValues(params[i], weights[i], 2U)
              == UMI_STATUS_OK);
        CHECK(UmiAiTrainingAdamWInspect(optimizers[i], &steps[i],
              &bytes[i]) == UMI_STATUS_OK);
        CHECK(steps[i] == 1U);
    }
    CHECK(memcmp(weights[0], weights[1], sizeof(weights[0])) == 0);
    CHECK(given[0] == 3.0F && given[1] == 4.0F);

    /* Reject a non-finite gradient before touching parameters, moments,
     * output or the optimiser's step count. */
    const float bad[] = {INFINITY, 1.0F};
    const float *invalid[] = {bad};
    UmiAiTrainingGradientClipReport sentinel = report;
    CHECK(UmiAiTrainingAdamWStepWithClippedGradients(optimizers[0], invalid,
          0.1F, 2.5F, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&report, &sentinel, sizeof(report)) == 0);
    CHECK(UmiAiTrainingAdamWInspect(optimizers[0], &steps[0],
          &bytes[0]) == UMI_STATUS_OK && steps[0] == 1U);
    for (size_t i = 0U; i < 2U; ++i) {
        UmiAiTrainingAdamWDestroy(optimizers[i]);
        UmiAiTrainingTapeDestroy(tapes[i]);
    }
    return 0;
}
