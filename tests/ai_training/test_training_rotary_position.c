/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_rotary_position.c
 * PURPOSE: Test RoPE rotation, partial rotary widths and relative shift parity.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_position.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

int main(void)
{
    const UmiAiTrainingRotaryConfig config = {6U, 4U, 10000.0};
    const float x[6] = {1.0F, 0.0F, 0.2F, 0.7F, -0.4F, 1.7F};
    const float y[6] = {0.5F, -0.25F, -0.3F, 0.4F, 0.7F, 0.2F};
    float output[6] = {0};
    float inPlace[6];
    double first = 0.0, shifted = 0.0, samePosition = 0.0;

    CHECK(UmiAiTrainingRotaryConfigValidate(&config) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingRotaryRotate(&config, x, 6U, 0U,
                                   output, 6U) == UMI_STATUS_OK);
    CHECK(memcmp(output, x, sizeof(x)) == 0);
    CHECK(UmiAiTrainingRotaryRotate(&config, x, 6U, 1U,
                                   output, 6U) == UMI_STATUS_OK);
    CHECK(fabsf(output[0] - (float)cos(1.0)) < 0.000001F);
    CHECK(fabsf(output[1] - (float)sin(1.0)) < 0.000001F);
    CHECK(output[4] == x[4] && output[5] == x[5]);
    memcpy(inPlace, x, sizeof(x));
    CHECK(UmiAiTrainingRotaryRotate(&config, inPlace, 6U, 1U,
                                   inPlace, 6U) == UMI_STATUS_OK);
    CHECK(memcmp(inPlace, output, sizeof(output)) == 0);
    CHECK(UmiAiTrainingRotaryRelativeDot(&config, x, y, 6U,
                                        6U, 2U, &first) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingRotaryRelativeDot(&config, x, y, 6U,
                                        106U, 102U, &shifted) == UMI_STATUS_OK);
    CHECK(fabs(first - shifted) < 0.00001);
    CHECK(UmiAiTrainingRotaryRelativeDot(&config, x, x, 6U,
                                        7000U, 7000U, &samePosition) == UMI_STATUS_OK);
    double norm = 0.0;
    for (size_t i = 0U; i < 6U; ++i)
        norm += (double)x[i] * (double)x[i];
    CHECK(fabs(samePosition - norm) < 0.00001);
    puts("rotary positions: identity, in-place, fixed tail, relative-shift invariant PASS");
    return 0;
}
