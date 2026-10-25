/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_rotary_rejections.c
 *
 * PURPOSE:
 *   Reject invalid RoPE and ring inputs without publishing partial
 *   vectors, corrupting state or consuming an absolute position.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_rotary_rejections.c
 * PURPOSE: Reject invalid RoPE and ring inputs without publishing partial
 *          vectors, corrupting state or consuming an absolute position.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_window.h"
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

int main(void)
{
    UmiAiTrainingRotaryConfig config = {4U, 4U, 10000.0};
    UmiAiTrainingRotaryWindowConfig windowConfig = {config, 2U};
    const float valid[4] = {0.1F, 0.3F, 0.9F, -0.7F};
    const float bad[4] = {0.0F, NAN, 0.0F, 0.0F};
    const float huge[4] = {FLT_MAX, FLT_MAX, 0.0F, 0.0F};
    float output[4];
    float saved[4];
    double score = 1234567.0;
    UmiAiTrainingRotaryWindow *window = NULL;
    UmiAiTrainingRotaryWindowStats stats, snapshot;
    UmiAiTrainingRotaryWindow *sentinel = (UmiAiTrainingRotaryWindow *)(uintptr_t)1U;

    for (size_t i = 0U; i < 4U; ++i) output[i] = 987.0F + (float)i;
    memcpy(saved, output, sizeof output);
    config.rotaryChannels = 3U;
    CHECK(UmiAiTrainingRotaryConfigValidate(&config) == UMI_STATUS_INVALID_ARGUMENT);
    config.rotaryChannels = 4U;
    config.frequencyBase = INFINITY;
    CHECK(UmiAiTrainingRotaryConfigValidate(&config) == UMI_STATUS_INVALID_ARGUMENT);
    config.frequencyBase = 10000.0;
    CHECK(UmiAiTrainingRotaryRotate(&config, valid, 3U, 1U, output, 4U)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingRotaryRotate(&config, valid, 4U, 1U, output, 3U)
          == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingRotaryRotate(&config, bad, 4U, 1U, output, 4U)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingRotaryRotate(&config, valid, 4U,
          UMI_AI_ROTARY_MAX_POSITION + 1U, output, 4U)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingRotaryRotate(&config, huge, 4U, 1U, output, 4U)
          == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(saved, output, sizeof saved) == 0);
    CHECK(UmiAiTrainingRotaryRelativeDot(&config, valid, bad, 4U,
          1U, 0U, &score) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(score == 1234567.0);

    windowConfig.contextCapacity = 0U;
    CHECK(UmiAiTrainingRotaryWindowCreate(&windowConfig, &sentinel)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(sentinel == (UmiAiTrainingRotaryWindow *)(uintptr_t)1U);
    windowConfig.contextCapacity = 2U;
    CHECK(UmiAiTrainingRotaryWindowCreate(&windowConfig, &window) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingRotaryWindowInspect(window, &stats) == UMI_STATUS_OK);
    snapshot = stats;
    CHECK(UmiAiTrainingRotaryWindowAppend(window, valid, bad, valid, 4U,
          output, 4U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingRotaryWindowAppend(window, valid, valid, valid, 4U,
          output, 3U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingRotaryWindowAppend(window, valid, valid, valid, 3U,
          output, 4U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingRotaryWindowAppend(window, valid, valid, valid, 4U,
          NULL, 4U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(saved, output, sizeof saved) == 0);
    CHECK(UmiAiTrainingRotaryWindowInspect(window, &stats) == UMI_STATUS_OK);
    CHECK(memcmp(&snapshot, &stats, sizeof stats) == 0);
    CHECK(UmiAiTrainingRotaryWindowAppend(window, valid, valid, valid, 4U,
          output, 4U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingRotaryWindowInspect(window, &stats) == UMI_STATUS_OK);
    CHECK(stats.computedRows == 1U && stats.nextAbsolutePosition == 1U);
    UmiAiTrainingRotaryWindowDestroy(window);
    puts("rotary research: failed operations retain caller outputs and cache state PASS");
    return 0;
}
