/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/stacked_kv_pool_fixture.h
 *
 * PURPOSE:
 *   Share small deterministic FP32 cache-pool fixtures and assertions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STACKED_KV_POOL_FIXTURE_H
#define UMICOM_STACKED_KV_POOL_FIXTURE_H
#include "umicom/ai/training/stacked_kv_pool.h"
#include <stdio.h>
#include <string.h>

#define VERIFY(predicate) do { if (!(predicate)) { \
    fprintf(stderr, "KV pool assertion %s:%d: %s\n", \
        __FILE__, __LINE__, #predicate); return 1; \
} } while (0)

static inline UmiAiTrainingStackedKvPoolConfig PoolTestConfig(void)
{
    UmiAiTrainingStackedKvPoolConfig cfg = {0};
    cfg.store.decoder = (UmiAiTrainingStackedDecoderConfig){
        260U, 4U, 8U, 12U, 2U, 2U, UINT64_C(2026)};
    cfg.store.checkpointSha256[0] = UINT8_C(42);
    cfg.maximumSessions = 3U;
    cfg.rawKvBudgetBytes = 1024U; /* 512 bytes per two-layer, four-row store. */
    return cfg;
}

static inline void PoolFill(float *dst, size_t count, float base)
{
    for (size_t i = 0U; i < count; ++i)
        dst[i] = base + (float)i * 0.125F;
}

#endif
