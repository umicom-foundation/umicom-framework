/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_kv_scheduler_boundaries.c
 *
 * PURPOSE:
 *   Qualify bounded queue rejection and byte-preserved caller output.
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
    const UmiAiTrainingStackedKvPoolConfig config = SchedulerPoolConfig(1U, 512U);
    UmiAiTrainingStackedKvPool *pool = NULL;
    UmiAiTrainingStackedKvScheduler *s = NULL;
    UmiAiTrainingStackedKvSchedulerTicket ticket = {0}, sentinel;
    UmiAiTrainingStackedKvSchedulerWork work;
    UmiAiTrainingStackedKvSchedulerStats stats;
    VERIFY(UmiAiTrainingStackedKvPoolCreate(&config, &pool) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerCreate(pool, 0U, &s) == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(s == NULL);
    VERIFY(UmiAiTrainingStackedKvSchedulerCreate(pool, 1U, &s) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerSubmit(s, 0U, 1U, &ticket) == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(UmiAiTrainingStackedKvSchedulerSubmit(s, 1U, 0U, &ticket) == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(UmiAiTrainingStackedKvSchedulerAdmitNext(s, &ticket) == UMI_STATUS_NOT_FOUND);
    memset(&work, 0xa5, sizeof(work));
    UmiAiTrainingStackedKvSchedulerWork priorWork = work;
    VERIFY(UmiAiTrainingStackedKvSchedulerDispatch(s, &work) == UMI_STATUS_UNAVAILABLE);
    VERIFY(memcmp(&work, &priorWork, sizeof(work)) == 0);
    VERIFY(UmiAiTrainingStackedKvSchedulerSubmit(s, 1U, 2U, &ticket) == UMI_STATUS_OK);
    sentinel = ticket;
    VERIFY(UmiAiTrainingStackedKvSchedulerSubmit(s, 1U, 2U, &ticket) == UMI_STATUS_ALREADY_EXISTS);
    VERIFY(ticket.generation == sentinel.generation);
    VERIFY(UmiAiTrainingStackedKvSchedulerSubmit(s, 2U, 1U, &ticket) == UMI_STATUS_CAPACITY_EXCEEDED);
    VERIFY(ticket.generation == sentinel.generation);
    VERIFY(UmiAiTrainingStackedKvSchedulerAdmitNext(s, &ticket) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerDispatch(s, &work) == UMI_STATUS_OK);
    memset(&stats, 0x5a, sizeof(stats));
    UmiAiTrainingStackedKvSchedulerStats previousStats = stats;
    VERIFY(UmiAiTrainingStackedKvSchedulerInspect(s, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(memcmp(&stats, &previousStats, sizeof(stats)) == 0);
    VERIFY(UmiAiTrainingStackedKvSchedulerAcknowledge(s, ticket) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerDispatch(s, &work) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerAcknowledge(s, ticket) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerAcknowledge(s, ticket) == UMI_STATUS_INVALID_STATE);
    VERIFY(UmiAiTrainingStackedKvSchedulerInspect(s, &stats) == UMI_STATUS_OK);
    VERIFY(stats.totalCompleted == 1U && stats.totalAcknowledgedSteps == 2U);
    UmiAiTrainingStackedKvSchedulerDestroy(s);
    UmiAiTrainingStackedKvPoolDestroy(pool);
    puts("KV scheduler: preserved output, completed lifetime and bound checks");
    return 0;
}
