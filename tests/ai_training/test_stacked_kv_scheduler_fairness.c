/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_kv_scheduler_fairness.c
 *
 * PURPOSE:
 *   Qualify FIFO admission and deterministic cooperative round-robin steps.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "stacked_kv_scheduler_fixture.h"
int main(void)
{
    const UmiAiTrainingStackedKvPoolConfig config = SchedulerPoolConfig(2U, 1024U);
    UmiAiTrainingStackedKvPool *pool = NULL;
    UmiAiTrainingStackedKvScheduler *s = NULL;
    UmiAiTrainingStackedKvSchedulerTicket a = {0}, b = {0}, c = {0}, admitted = {0};
    const uint64_t expected[] = {101U, 202U, 101U, 202U, 303U};
    VERIFY(UmiAiTrainingStackedKvPoolCreate(&config, &pool) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerCreate(pool, 3U, &s) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerSubmit(s, 101U, 2U, &a) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerSubmit(s, 202U, 2U, &b) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerSubmit(s, 303U, 1U, &c) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerAdmitNext(s, &admitted) == UMI_STATUS_OK);
    VERIFY(admitted.slotIndex == a.slotIndex && admitted.generation == a.generation);
    VERIFY(UmiAiTrainingStackedKvSchedulerAdmitNext(s, &admitted) == UMI_STATUS_OK);
    VERIFY(admitted.slotIndex == b.slotIndex && admitted.generation == b.generation);
    VERIFY(UmiAiTrainingStackedKvSchedulerAdmitNext(s, &admitted) == UMI_STATUS_CAPACITY_EXCEEDED);
    for (size_t i = 0U; i < sizeof(expected) / sizeof(expected[0]); ++i) {
        UmiAiTrainingStackedKvSchedulerWork work = {0};
        if (i == 4U) {
            VERIFY(UmiAiTrainingStackedKvSchedulerAdmitNext(s, &admitted) == UMI_STATUS_OK);
            VERIFY(admitted.slotIndex == c.slotIndex);
        }
        VERIFY(UmiAiTrainingStackedKvSchedulerDispatch(s, &work) == UMI_STATUS_OK);
        VERIFY(work.requestId == expected[i]);
        VERIFY(work.requestedSteps == (i == 4U ? 1U : 2U));
        VERIFY(UmiAiTrainingStackedKvSchedulerAcknowledge(s, work.ticket) == UMI_STATUS_OK);
    }
    UmiAiTrainingStackedKvSchedulerStats stats = {0};
    UmiAiTrainingStackedKvPoolStats ps = {0};
    VERIFY(UmiAiTrainingStackedKvSchedulerInspect(s, &stats) == UMI_STATUS_OK);
    VERIFY(stats.totalCompleted == 3U && stats.totalDispatches == 5U);
    VERIFY(stats.waitingRequests == 0U && stats.readyRequests == 0U);
    VERIFY(UmiAiTrainingStackedKvPoolInspect(pool, &ps) == UMI_STATUS_OK);
    VERIFY(ps.activeSessions == 0U && ps.totalAcquisitions == 3U);
    UmiAiTrainingStackedKvSchedulerDestroy(s);
    UmiAiTrainingStackedKvPoolDestroy(pool);
    puts("KV scheduler: FIFO admission and deterministic round robin");
    return 0;
}
