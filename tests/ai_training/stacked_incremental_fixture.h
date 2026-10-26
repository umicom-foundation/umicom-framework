/*-----------------------------------------------------------------------------
 * Umicom Framework | shared deterministic C23 stacked-inference fixtures
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STACKED_INCREMENTAL_FIXTURE_H
#define UMICOM_STACKED_INCREMENTAL_FIXTURE_H
#include "umicom/ai/training/stacked_kv_incremental.h"
#include "umicom/ai/training/stacked_prefix_inference.h"
#include "umicom/ai/training/stacked_projection_trace.h"
#include "umicom/ai/training/stacked_checkpoint.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERIFY(condition) do { if (!(condition)) { \
    fprintf(stderr, "incremental test line %d: %s\n", __LINE__, #condition); \
    return 1; } } while (0)

static inline UmiAiTrainingStackedDecoderConfig IncrementalConfig(
    uint32_t layers, uint32_t context)
{
    const UmiAiTrainingStackedDecoderConfig config = {
        260U, context, 8U, 12U, 2U, layers, UINT64_C(19383)
    };
    return config;
}

static inline int IncrementalNear(float a, float b)
{
    return isfinite(a) && isfinite(b) && fabsf(a - b) <= 0.00010F;
}

#endif
