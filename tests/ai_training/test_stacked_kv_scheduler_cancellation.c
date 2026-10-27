/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_kv_scheduler_cancellation.c
 *
 * PURPOSE:
 *   Reject stale/cross-owner tickets and preserve in-flight work on cancel.
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
    UmiAiTrainingStackedKvScheduler *s = NULL, *foreign = NULL;
    UmiAiTrainingStackedKvSchedulerTicket a = {0}, b = {0}, c = {0};
    UmiAiTrainingStackedKvSchedulerTicket admitted = {0};
    UmiAiTrainingStackedKvSchedulerWork work = {0};
    UmiAiTrainingStackedKvSchedulerStats stats = {0};
    VERIFY(UmiAiTrainingStackedKvPoolCreate(&config, &pool) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerCreate(pool, 2U, &s) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerCreate(pool, 1U, &foreign) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerSubmit(s, 7U, 2U, &a) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerSubmit(s, 8U, 1U, &b) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerCancel(s, b) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerCancel(foreign, a) == UMI_STATUS_INVALID_STATE);
    VERIFY(UmiAiTrainingStackedKvSchedulerAdmitNext(s, &admitted) == UMI_STATUS_OK);
    VERIFY(admitted.slotIndex == a.slotIndex);
    VERIFY(UmiAiTrainingStackedKvSchedulerDispatch(s, &work) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerCancel(s, a) == UMI_STATUS_BUSY);
    VERIFY(UmiAiTrainingStackedKvSchedulerDispatch(s, &work) == UMI_STATUS_BUSY);
    VERIFY(UmiAiTrainingStackedKvSchedulerReturnUncommitted(s, a) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerDispatch(s, &work) == UMI_STATUS_OK);
    VERIFY(work.completedSteps == 0U);
    VERIFY(UmiAiTrainingStackedKvSchedulerAcknowledge(s, a) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerCancel(s, a) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerSubmit(s, 9U, 1U, &c) == UMI_STATUS_OK);
    VERIFY(c.slotIndex == a.slotIndex || c.slotIndex == b.slotIndex);
    VERIFY(UmiAiTrainingStackedKvSchedulerCancel(s, a) == UMI_STATUS_INVALID_STATE);
    VERIFY(UmiAiTrainingStackedKvSchedulerCancel(s, c) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerInspect(s, &stats) == UMI_STATUS_OK);
    VERIFY(stats.totalCancelled == 3U && stats.totalAcknowledgedSteps == 1U);
    UmiAiTrainingStackedKvSchedulerDestroy(foreign);
    UmiAiTrainingStackedKvSchedulerDestroy(s);
    UmiAiTrainingStackedKvPoolDestroy(pool);
    puts("KV scheduler: cancellation and stale/cross-scheduler tickets");
    return 0;
}
