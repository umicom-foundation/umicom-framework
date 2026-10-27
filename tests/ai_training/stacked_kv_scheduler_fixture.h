/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/stacked_kv_scheduler_fixture.h
 *
 * PURPOSE:
 *   Share deterministic native KV scheduler models and assertion helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STACKED_KV_SCHEDULER_FIXTURE_H
#define UMICOM_STACKED_KV_SCHEDULER_FIXTURE_H

#include "umicom/ai/training/stacked_kv_scheduler.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define VERIFY(test) do { if (!(test)) { \
    fprintf(stderr, "scheduler test failed at line %d: %s\n", __LINE__, #test); \
    return 1; } } while (0)

static inline UmiAiTrainingStackedKvPoolConfig SchedulerPoolConfig(
    size_t sessions, size_t budget)
{
    UmiAiTrainingStackedKvPoolConfig config = {0};
    config.store.decoder = (UmiAiTrainingStackedDecoderConfig){
        260U, 4U, 8U, 12U, 2U, 2U, UINT64_C(361)};
    config.store.checkpointSha256[0] = UINT8_C(31);
    config.maximumSessions = sessions;
    config.rawKvBudgetBytes = budget;
    return config;
}

#endif /* UMICOM_STACKED_KV_SCHEDULER_FIXTURE_H */
