/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_kv_scheduler.c
 *
 * PURPOSE:
 *   Compose bounded FIFO admission and one-step round-robin dispatch around
 *   the established KV lease pool without duplicating any Transformer maths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_kv_scheduler.h"

#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum SchedulerEntryState {
    SCHEDULER_EMPTY = 0,
    SCHEDULER_WAITING,
    SCHEDULER_READY,
    SCHEDULER_DISPATCHED
} SchedulerEntryState;

typedef struct SchedulerEntry {
    SchedulerEntryState state;
    uint64_t generation;
    uint64_t requestId;
    uint64_t submissionOrder;
    size_t requestedSteps;
    size_t completedSteps;
    UmiAiTrainingStackedKvLease lease;
} SchedulerEntry;

struct UmiAiTrainingStackedKvScheduler {
    UmiAiTrainingStackedKvPool *pool; /* Borrowed. */
    uint64_t identity;
    size_t capacity;
    size_t previousDispatched;
    size_t inFlight; /* SIZE_MAX denotes no outstanding work. */
    uint64_t submitted;
    uint64_t admitted;
    uint64_t dispatched;
    uint64_t acknowledged;
    uint64_t completed;
    uint64_t cancelled;
    SchedulerEntry entries[];
};

/* A process-wide monotonic identifier prevents a destroyed scheduler's
 * ticket from being accepted by a new scheduler at the same heap address.
 * Never wrap the identity counter; non-thread-safe scheduler use is still
 * required because the individual pool and model are caller-serialised. */
static _Atomic uint64_t nextSchedulerIdentity = UINT64_C(1);

static uint64_t SchedulerNextIdentity(void)
{
    uint64_t current = atomic_load(&nextSchedulerIdentity);
    for (;;) {
        if (current == 0U || current == UINT64_MAX) return UINT64_C(0);
        if (atomic_compare_exchange_weak(&nextSchedulerIdentity,
                                         &current, current + UINT64_C(1)))
            return current;
    }
}

static UmiAiTrainingStackedKvSchedulerTicket SchedulerTicket(
    const UmiAiTrainingStackedKvScheduler *scheduler, size_t slot)
{
    const UmiAiTrainingStackedKvSchedulerTicket ticket = {
        scheduler->identity,
        scheduler->entries[slot].generation,
        slot
    };
    return ticket;
}

static SchedulerEntry *SchedulerFind(
    UmiAiTrainingStackedKvScheduler *scheduler,
    UmiAiTrainingStackedKvSchedulerTicket ticket)
{
    if (scheduler == NULL || ticket.schedulerIdentity != scheduler->identity ||
        ticket.slotIndex >= scheduler->capacity) return NULL;
    SchedulerEntry *entry = &scheduler->entries[ticket.slotIndex];
    if (entry->state == SCHEDULER_EMPTY ||
        entry->generation != ticket.generation) return NULL;
    return entry;
}

UmiStatus UmiAiTrainingStackedKvSchedulerCreate(
    UmiAiTrainingStackedKvPool *borrowedPool, size_t maximumRequests,
    UmiAiTrainingStackedKvScheduler **outScheduler)
{
    UmiAiTrainingStackedKvPoolStats poolStats = {0};
    if (borrowedPool == NULL || outScheduler == NULL || *outScheduler != NULL ||
        maximumRequests == 0U || maximumRequests > 1024U ||
        maximumRequests >
            (SIZE_MAX - sizeof(UmiAiTrainingStackedKvScheduler)) /
            sizeof(SchedulerEntry)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiTrainingStackedKvPoolInspect(borrowedPool, &poolStats);
    if (status != UMI_STATUS_OK) return status;
    UmiAiTrainingStackedKvScheduler *created = calloc(1U,
        sizeof(*created) + maximumRequests * sizeof(SchedulerEntry));
    if (created == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    created->identity = SchedulerNextIdentity();
    if (created->identity == 0U) {
        free(created);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    created->pool = borrowedPool;
    created->capacity = maximumRequests;
    created->previousDispatched = maximumRequests - 1U;
    created->inFlight = SIZE_MAX;
    *outScheduler = created;
    return UMI_STATUS_OK;
}

void UmiAiTrainingStackedKvSchedulerDestroy(
    UmiAiTrainingStackedKvScheduler *scheduler)
{
    if (scheduler == NULL) return;
    /* The application has stopped every outstanding computation. A failed
     * Release is not recoverable via this void destructor; pool ownership
     * remains with the caller and the pool must be destroyed afterwards. */
    for (size_t slot = 0U; slot < scheduler->capacity; ++slot) {
        SchedulerEntry *entry = &scheduler->entries[slot];
        if (entry->state == SCHEDULER_READY ||
            entry->state == SCHEDULER_DISPATCHED)
            (void)UmiAiTrainingStackedKvPoolRelease(scheduler->pool, entry->lease);
    }
    free(scheduler);
}

UmiStatus UmiAiTrainingStackedKvSchedulerSubmit(
    UmiAiTrainingStackedKvScheduler *scheduler,
    uint64_t requestId, size_t requestedSteps,
    UmiAiTrainingStackedKvSchedulerTicket *outTicket)
{
    size_t available = SIZE_MAX;
    if (scheduler == NULL || outTicket == NULL || requestId == 0U ||
        requestedSteps == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (scheduler->submitted == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t slot = 0U; slot < scheduler->capacity; ++slot) {
        const SchedulerEntry *entry = &scheduler->entries[slot];
        if (entry->state != SCHEDULER_EMPTY && entry->requestId == requestId)
            return UMI_STATUS_ALREADY_EXISTS;
        if (entry->state == SCHEDULER_EMPTY && available == SIZE_MAX &&
            entry->generation != UINT64_MAX) available = slot;
    }
    if (available == SIZE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    SchedulerEntry *entry = &scheduler->entries[available];
    ++entry->generation;
    ++scheduler->submitted;
    entry->submissionOrder = scheduler->submitted;
    entry->requestId = requestId;
    entry->requestedSteps = requestedSteps;
    entry->completedSteps = 0U;
    entry->state = SCHEDULER_WAITING;
    *outTicket = SchedulerTicket(scheduler, available);
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedKvSchedulerAdmitNext(
    UmiAiTrainingStackedKvScheduler *scheduler,
    UmiAiTrainingStackedKvSchedulerTicket *outTicket)
{
    size_t oldest = SIZE_MAX;
    uint64_t order = UINT64_MAX;
    UmiAiTrainingStackedKvLease lease = {0};
    if (scheduler == NULL || outTicket == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (scheduler->admitted == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t slot = 0U; slot < scheduler->capacity; ++slot) {
        const SchedulerEntry *entry = &scheduler->entries[slot];
        if (entry->state == SCHEDULER_WAITING &&
            entry->submissionOrder < order) {
            oldest = slot;
            order = entry->submissionOrder;
        }
    }
    if (oldest == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* The existing pool owns allocation/budget/idle-store admission. */
    UmiStatus status = UmiAiTrainingStackedKvPoolAcquire(scheduler->pool, &lease);
    if (status != UMI_STATUS_OK) return status;
    SchedulerEntry *entry = &scheduler->entries[oldest];
    entry->lease = lease;
    entry->state = SCHEDULER_READY;
    ++scheduler->admitted;
    *outTicket = SchedulerTicket(scheduler, oldest);
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedKvSchedulerDispatch(
    UmiAiTrainingStackedKvScheduler *scheduler,
    UmiAiTrainingStackedKvSchedulerWork *outWork)
{
    if (scheduler == NULL || outWork == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (scheduler->inFlight != SIZE_MAX) return UMI_STATUS_BUSY;
    if (scheduler->dispatched == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t offset = 1U; offset <= scheduler->capacity; ++offset) {
        const size_t slot =
            (scheduler->previousDispatched + offset) % scheduler->capacity;
        SchedulerEntry *entry = &scheduler->entries[slot];
        if (entry->state != SCHEDULER_READY) continue;
        const UmiAiTrainingStackedKvSchedulerWork work = {
            SchedulerTicket(scheduler, slot),
            entry->lease,
            entry->requestId,
            entry->completedSteps,
            entry->requestedSteps
        };
        entry->state = SCHEDULER_DISPATCHED;
        scheduler->inFlight = slot;
        scheduler->previousDispatched = slot;
        ++scheduler->dispatched;
        *outWork = work;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_UNAVAILABLE;
}

UmiStatus UmiAiTrainingStackedKvSchedulerAcknowledge(
    UmiAiTrainingStackedKvScheduler *scheduler,
    UmiAiTrainingStackedKvSchedulerTicket ticket)
{
    SchedulerEntry *entry = SchedulerFind(scheduler, ticket);
    if (entry == NULL || entry->state != SCHEDULER_DISPATCHED ||
        scheduler->inFlight != ticket.slotIndex) return UMI_STATUS_INVALID_STATE;
    if (scheduler->acknowledged == UINT64_MAX ||
        (entry->completedSteps + 1U == entry->requestedSteps &&
         scheduler->completed == UINT64_MAX)) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (entry->completedSteps + 1U == entry->requestedSteps) {
        /* Release before changing scheduler state; the K/V pool implements
         * the actual lease invalidation, reset, and optional idle reuse. */
        UmiStatus status = UmiAiTrainingStackedKvPoolRelease(
            scheduler->pool, entry->lease);
        if (status != UMI_STATUS_OK) return status;
        entry->state = SCHEDULER_EMPTY;
        ++scheduler->completed;
    } else {
        ++entry->completedSteps;
        entry->state = SCHEDULER_READY;
    }
    ++scheduler->acknowledged;
    scheduler->inFlight = SIZE_MAX;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedKvSchedulerReturnUncommitted(
    UmiAiTrainingStackedKvScheduler *scheduler,
    UmiAiTrainingStackedKvSchedulerTicket ticket)
{
    SchedulerEntry *entry = SchedulerFind(scheduler, ticket);
    if (entry == NULL || entry->state != SCHEDULER_DISPATCHED ||
        scheduler->inFlight != ticket.slotIndex) return UMI_STATUS_INVALID_STATE;
    entry->state = SCHEDULER_READY;
    scheduler->inFlight = SIZE_MAX;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedKvSchedulerCancel(
    UmiAiTrainingStackedKvScheduler *scheduler,
    UmiAiTrainingStackedKvSchedulerTicket ticket)
{
    SchedulerEntry *entry = SchedulerFind(scheduler, ticket);
    if (entry == NULL) return UMI_STATUS_INVALID_STATE;
    if (entry->state == SCHEDULER_DISPATCHED) return UMI_STATUS_BUSY;
    if (scheduler->cancelled == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (entry->state == SCHEDULER_READY) {
        UmiStatus status = UmiAiTrainingStackedKvPoolRelease(
            scheduler->pool, entry->lease);
        if (status != UMI_STATUS_OK) return status;
    }
    entry->state = SCHEDULER_EMPTY;
    ++scheduler->cancelled;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedKvSchedulerInspect(
    const UmiAiTrainingStackedKvScheduler *scheduler,
    UmiAiTrainingStackedKvSchedulerStats *outStats)
{
    UmiAiTrainingStackedKvPoolStats pool = {0};
    UmiAiTrainingStackedKvSchedulerStats result = {0};
    if (scheduler == NULL || outStats == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiTrainingStackedKvPoolInspect(scheduler->pool, &pool);
    if (status != UMI_STATUS_OK) return status;
    result.maximumRequests = scheduler->capacity;
    result.totalSubmitted = scheduler->submitted;
    result.totalAdmitted = scheduler->admitted;
    result.totalDispatches = scheduler->dispatched;
    result.totalAcknowledgedSteps = scheduler->acknowledged;
    result.totalCompleted = scheduler->completed;
    result.totalCancelled = scheduler->cancelled;
    result.activeRawKvBytes = pool.reservedRawKvBytes;
    result.idleRawKvBytes = pool.idleRawKvBytes;
    result.physicalRawKvBytes = pool.physicalRawKvBytes;
    for (size_t slot = 0U; slot < scheduler->capacity; ++slot) {
        const SchedulerEntryState state = scheduler->entries[slot].state;
        if (state == SCHEDULER_WAITING) ++result.waitingRequests;
        if (state == SCHEDULER_READY) ++result.readyRequests;
        if (state == SCHEDULER_DISPATCHED) ++result.dispatchedRequests;
    }
    *outStats = result;
    return UMI_STATUS_OK;
}
