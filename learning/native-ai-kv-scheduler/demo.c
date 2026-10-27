/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-ai-kv-scheduler/demo.c
 *
 * PURPOSE:
 *   Demonstrate FIFO KV admission and alternating cooperative work dispatch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_kv_scheduler.h"
#include <inttypes.h>
#include <stdio.h>

int main(void)
{
    UmiAiTrainingStackedKvPoolConfig configuration = {0};
    UmiAiTrainingStackedKvPool *pool = NULL;
    UmiAiTrainingStackedKvScheduler *scheduler = NULL;
    UmiAiTrainingStackedKvSchedulerTicket a = {0}, b = {0}, admitted = {0};
    configuration.store.decoder = (UmiAiTrainingStackedDecoderConfig){
        260U, 4U, 8U, 12U, 2U, 2U, UINT64_C(2026)};
    configuration.store.checkpointSha256[0] = UINT8_C(1);
    configuration.maximumSessions = 2U;
    configuration.rawKvBudgetBytes = 1024U;
    if (UmiAiTrainingStackedKvPoolCreate(&configuration, &pool) != UMI_STATUS_OK)
        return 1;
    if (UmiAiTrainingStackedKvSchedulerCreate(pool, 2U, &scheduler) != UMI_STATUS_OK ||
        UmiAiTrainingStackedKvSchedulerSubmit(scheduler, 100U, 2U, &a) != UMI_STATUS_OK ||
        UmiAiTrainingStackedKvSchedulerSubmit(scheduler, 200U, 2U, &b) != UMI_STATUS_OK ||
        UmiAiTrainingStackedKvSchedulerAdmitNext(scheduler, &admitted) != UMI_STATUS_OK ||
        (admitted.generation != a.generation || admitted.slotIndex != a.slotIndex) ||
        UmiAiTrainingStackedKvSchedulerAdmitNext(scheduler, &admitted) != UMI_STATUS_OK ||
        (admitted.generation != b.generation || admitted.slotIndex != b.slotIndex)) {
        UmiAiTrainingStackedKvSchedulerDestroy(scheduler);
        UmiAiTrainingStackedKvPoolDestroy(pool);
        return 1;
    }
    for (size_t i = 0U; i < 4U; ++i) {
        UmiAiTrainingStackedKvSchedulerWork work = {0};
        if (UmiAiTrainingStackedKvSchedulerDispatch(scheduler, &work) != UMI_STATUS_OK) {
            UmiAiTrainingStackedKvSchedulerDestroy(scheduler);
            UmiAiTrainingStackedKvPoolDestroy(pool);
            return 1;
        }
        /* This teaching demo ACKs an abstract work unit. A real application
         * would first execute and commit one token with its decoder session. */
        printf("request=%" PRIu64 " step=%zu/%zu\n", work.requestId,
               work.completedSteps + 1U, work.requestedSteps);
        if (UmiAiTrainingStackedKvSchedulerAcknowledge(scheduler, work.ticket) !=
            UMI_STATUS_OK) {
            UmiAiTrainingStackedKvSchedulerDestroy(scheduler);
            UmiAiTrainingStackedKvPoolDestroy(pool);
            return 1;
        }
    }
    UmiAiTrainingStackedKvSchedulerDestroy(scheduler);
    UmiAiTrainingStackedKvPoolDestroy(pool);
    return 0;
}
