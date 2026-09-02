/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/resilience/bulkhead.c
 *
 * PURPOSE:
 *   Implement concurrency and waiting admission accounting for isolated
 *   workloads.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/resilience/bulkhead.h"

#include <stddef.h>
#include <stdlib.h>
#include "umicom/platform/threading.h"
struct UmiBulkhead { UmiBulkheadConfig config; UmiBulkheadSnapshot state; UmiMutex *mutex; };
/*
 * Initialise bulkhead from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_bulkhead_create(const UmiBulkheadConfig *config, UmiBulkhead **out_bulkhead)
{
    UmiBulkhead *b; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config == NULL || out_bulkhead == NULL || config->maximum_concurrent == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    *out_bulkhead = NULL; b = (UmiBulkhead *)calloc(1U, sizeof(*b)); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (b == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&b->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) { free(b); return status; }
    b->config = *config; *out_bulkhead = b; return UMI_STATUS_OK;
}
/* Release or reset state held by bulkhead so the same storage can be reused safely. */
void umi_bulkhead_destroy(UmiBulkhead *bulkhead) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (bulkhead != NULL) { umi_mutex_destroy(bulkhead->mutex); free(bulkhead); } }
/* Provide the bulkhead enter operation used by this module and its client applications. */
UmiStatus umi_bulkhead_enter(UmiBulkhead *bulkhead, int allow_waiting)
{
    UmiStatus result = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bulkhead == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_mutex_lock(bulkhead->mutex) != UMI_STATUS_OK) return UMI_STATUS_INTERNAL_ERROR;
    /* Apply this operation only while the related capability or state is available. */
    if (bulkhead->state.active < bulkhead->config.maximum_concurrent) ++bulkhead->state.active;
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (allow_waiting && bulkhead->state.waiting < bulkhead->config.maximum_waiting) { ++bulkhead->state.waiting; result = UMI_STATUS_BUSY; }
    /* Use this fallback path when the earlier condition does not apply. */
    else { ++bulkhead->state.rejected; result = UMI_STATUS_CAPACITY_EXCEEDED; }
    (void)umi_mutex_unlock(bulkhead->mutex); return result;
}
/* Provide the bulkhead leave operation used by this module and its client applications. */
void umi_bulkhead_leave(UmiBulkhead *bulkhead)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (bulkhead != NULL && umi_mutex_lock(bulkhead->mutex) == UMI_STATUS_OK) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (bulkhead->state.active > 0U) --bulkhead->state.active; (void)umi_mutex_unlock(bulkhead->mutex); } }
/*
 * Provide the bulkhead waiting done operation used by this module and its client
 * applications.
 */
void umi_bulkhead_waiting_done(UmiBulkhead *bulkhead)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (bulkhead != NULL && umi_mutex_lock(bulkhead->mutex) == UMI_STATUS_OK) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (bulkhead->state.waiting > 0U) --bulkhead->state.waiting; (void)umi_mutex_unlock(bulkhead->mutex); } }
/* Provide the bulkhead snapshot operation used by this module and its client applications. */
UmiBulkheadSnapshot umi_bulkhead_snapshot(UmiBulkhead *bulkhead)
{ UmiBulkheadSnapshot s = {0}; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (bulkhead != NULL && umi_mutex_lock(bulkhead->mutex) == UMI_STATUS_OK) { s = bulkhead->state; (void)umi_mutex_unlock(bulkhead->mutex); } return s; }
