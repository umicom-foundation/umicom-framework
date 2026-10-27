/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_kv_scheduler_admission.c
 *
 * PURPOSE:
 *   Verify full-pool backpressure and explicit reuse of canonical idle stores.
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
    UmiAiTrainingStackedKvSchedulerTicket first = {0}, second = {0};
    UmiAiTrainingStackedKvSchedulerTicket admitted = {0};
    UmiAiTrainingStackedKvSchedulerWork one = {0}, two = {0};
    UmiAiTrainingStackedKvPoolStats ps = {0};
    VERIFY(UmiAiTrainingStackedKvPoolCreate(&config, &pool) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolSetIdleRetention(pool, 1U) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerCreate(pool, 2U, &s) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerSubmit(s, 1U, 1U, &first) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerSubmit(s, 2U, 1U, &second) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvSchedulerAdmitNext(s, &admitted) == UMI_STATUS_OK);
    VERIFY(admitted.slotIndex == first.slotIndex);
    VERIFY(UmiAiTrainingStackedKvSchedulerAdmitNext(s, &admitted) == UMI_STATUS_CAPACITY_EXCEEDED);
    VERIFY(UmiAiTrainingStackedKvSchedulerDispatch(s, &one) == UMI_STATUS_OK);
    VERIFY(one.requestId == 1U);
    VERIFY(UmiAiTrainingStackedKvSchedulerAcknowledge(s, one.ticket) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolInspect(pool, &ps) == UMI_STATUS_OK);
    VERIFY(ps.idleStores == 1U && ps.physicalRawKvBytes == 512U &&
           ps.activeSessions == 0U && ps.newStoreAllocations == 1U);
    VERIFY(UmiAiTrainingStackedKvSchedulerAdmitNext(s, &admitted) == UMI_STATUS_OK);
    VERIFY(admitted.slotIndex == second.slotIndex);
    VERIFY(UmiAiTrainingStackedKvPoolInspect(pool, &ps) == UMI_STATUS_OK);
    VERIFY(ps.idleStoreReuses == 1U && ps.newStoreAllocations == 1U);
    VERIFY(UmiAiTrainingStackedKvSchedulerDispatch(s, &two) == UMI_STATUS_OK);
    VERIFY(two.requestId == 2U);
    VERIFY(UmiAiTrainingStackedKvPoolInspectLease(pool, one.lease,
        &(UmiAiTrainingStackedKvStoreInfo){0}) == UMI_STATUS_INVALID_STATE);
    VERIFY(UmiAiTrainingStackedKvSchedulerAcknowledge(s, two.ticket) == UMI_STATUS_OK);
    UmiAiTrainingStackedKvSchedulerDestroy(s);
    UmiAiTrainingStackedKvPoolDestroy(pool);
    puts("KV scheduler: physical budget, FIFO backpressure and warm allocation");
    return 0;
}
