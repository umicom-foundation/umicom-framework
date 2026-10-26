/*-----------------------------------------------------------------------------
 * Umicom Framework | reusable native stacked-decoder test fixtures
 * AUTHOR: Sammy Hegab / Umicom Foundation | LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STACKED_TEST_FIXTURE_H
#define UMICOM_STACKED_TEST_FIXTURE_H
#include "umicom/ai/training/stacked_decoder.h"
#include "umicom/ai/training/stacked_checkpoint.h"
#include "umicom/ai/training/stacked_experiment.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "stacked regression line %d: %s\n", __LINE__, #condition); \
        return 1; \
    } \
} while (0)
static inline UmiAiTrainingStackedDecoderConfig TestStackedConfig(void)
{
    return (UmiAiTrainingStackedDecoderConfig){
        16U, 4U, 8U, 12U, 2U, 2U, UINT64_C(1310)
    };
}
static inline void TestStackedWindow(
    size_t inputs[4], size_t targets[4], uint8_t masks[4])
{
    const size_t i[4] = {1U, 5U, 6U, 7U};
    const size_t t[4] = {0U, 6U, 7U, 2U};
    const uint8_t m[4] = {0U, 1U, 1U, 1U};
    memcpy(inputs, i, sizeof(i));
    memcpy(targets, t, sizeof(t));
    memcpy(masks, m, sizeof(m));
}
static inline void TestStackedSourceDigest(
    uint8_t out[UMI_SHA256_BYTES], uint8_t seed)
{
    for (size_t i = 0U; i < UMI_SHA256_BYTES; ++i)
        out[i] = (uint8_t)(seed + (uint8_t)i);
}
#endif
