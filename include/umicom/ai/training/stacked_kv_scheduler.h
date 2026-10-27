/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_kv_scheduler.h
 *
 * PURPOSE:
 *   Admit bounded KV-backed requests in FIFO order and dispatch cooperative
 *   single-step work in deterministic round-robin order.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_KV_SCHEDULER_H
#define UMICOM_AI_TRAINING_STACKED_KV_SCHEDULER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/stacked_kv_pool.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingStackedKvScheduler UmiAiTrainingStackedKvScheduler;

/* Slot and generation must both match; foreign scheduler IDs are rejected.
 * A ticket is not itself a reference to a K/V store or a model. */
typedef struct UmiAiTrainingStackedKvSchedulerTicket {
    uint64_t schedulerIdentity;
    uint64_t generation;
    size_t slotIndex;
} UmiAiTrainingStackedKvSchedulerTicket;

/* The caller owns the borrowed pool, decoder, and all token-generation work.
 * The pool must outlive the scheduler, which releases its active leases on
 * destruction. This is a cooperative, synchronous dispatcher, not a thread
 * pool or a GPU batching service. */
typedef struct UmiAiTrainingStackedKvSchedulerWork {
    UmiAiTrainingStackedKvSchedulerTicket ticket;
    UmiAiTrainingStackedKvLease lease;
    uint64_t requestId;
    size_t completedSteps;
    size_t requestedSteps;
} UmiAiTrainingStackedKvSchedulerWork;

typedef struct UmiAiTrainingStackedKvSchedulerStats {
    size_t maximumRequests;
    size_t waitingRequests;
    size_t readyRequests;
    size_t dispatchedRequests;
    uint64_t totalSubmitted;
    uint64_t totalAdmitted;
    uint64_t totalDispatches;
    uint64_t totalAcknowledgedSteps;
    uint64_t totalCompleted;
    uint64_t totalCancelled;
    size_t activeRawKvBytes;
    size_t idleRawKvBytes;
    size_t physicalRawKvBytes;
} UmiAiTrainingStackedKvSchedulerStats;

/* Initialise *outScheduler=NULL. 1..1024 request slots are supported. This
 * service does not create another KV pool, compute logits, or copy a model. */
UmiStatus UmiAiTrainingStackedKvSchedulerCreate(
    UmiAiTrainingStackedKvPool *borrowedPool, size_t maximumRequests,
    UmiAiTrainingStackedKvScheduler **outScheduler);
void UmiAiTrainingStackedKvSchedulerDestroy(
    UmiAiTrainingStackedKvScheduler *scheduler);

/* Queue one request (1..SIZE_MAX declared work units) without reserving KV.
 * requestId must be nonzero and unique among currently live requests.
 * Tickets and all existing queue state are unchanged after rejection. */
UmiStatus UmiAiTrainingStackedKvSchedulerSubmit(
    UmiAiTrainingStackedKvScheduler *scheduler,
    uint64_t requestId, size_t requestedSteps,
    UmiAiTrainingStackedKvSchedulerTicket *outTicket);

/* Admit exactly ONE oldest waiting request through the existing pool's
 * Acquire; a full pool returns CAPACITY_EXCEEDED with no queue mutation.
 * The caller can retry when a completing/cancelled request frees capacity.
 * A returned ticket is the original ticket, not a new ownership identity. */
UmiStatus UmiAiTrainingStackedKvSchedulerAdmitNext(
    UmiAiTrainingStackedKvScheduler *scheduler,
    UmiAiTrainingStackedKvSchedulerTicket *outTicket);

/* Dispatch exactly one ready request. It becomes IN-FLIGHT until acknowledged
 * or explicitly returned uncommitted. There may be only one in-flight work
 * unit per scheduler. Selection rotates over ready slot indexes. UNAVAILABLE
 * means no admitted request is ready; BUSY means work is still in flight.
 * Returns a borrowed lease for a synchronous, caller-owned model operation.
 * The caller must verify decoder identity and own any inference transaction. */
UmiStatus UmiAiTrainingStackedKvSchedulerDispatch(
    UmiAiTrainingStackedKvScheduler *scheduler,
    UmiAiTrainingStackedKvSchedulerWork *outWork);

/* Mark work complete ONLY after the caller successfully commits that logical
 * step in its own inference session. This service cannot make neural-model
 * state atomic. The final step releases the existing pool lease before
 * freeing the scheduler slot. Invalid tickets leave all counters unchanged. */
UmiStatus UmiAiTrainingStackedKvSchedulerAcknowledge(
    UmiAiTrainingStackedKvScheduler *scheduler,
    UmiAiTrainingStackedKvSchedulerTicket ticket);

/* Return the dispatched item WITHOUT incrementing completedSteps, provided
 * the caller independently guarantees it made NO committed inference change.
 * Never use this API to silently re-run a partially committed model step. */
UmiStatus UmiAiTrainingStackedKvSchedulerReturnUncommitted(
    UmiAiTrainingStackedKvScheduler *scheduler,
    UmiAiTrainingStackedKvSchedulerTicket ticket);

/* Cancel waiting or ready work. In-flight cancellation is BUSY: stop the
 * worker first and return its uncommitted unit when that is safe. A ready
 * cancellation releases the canonical pool lease. Stale/foreign tickets
 * never affect another request. */
UmiStatus UmiAiTrainingStackedKvSchedulerCancel(
    UmiAiTrainingStackedKvScheduler *scheduler,
    UmiAiTrainingStackedKvSchedulerTicket ticket);

/* Aggregate scheduler counters with the existing pool's actual reservation
 * counters. Output stays byte-for-byte untouched on rejection. */
UmiStatus UmiAiTrainingStackedKvSchedulerInspect(
    const UmiAiTrainingStackedKvScheduler *scheduler,
    UmiAiTrainingStackedKvSchedulerStats *outStats);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_STACKED_KV_SCHEDULER_H */
