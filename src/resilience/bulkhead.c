/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/resilience/bulkhead.c
 *
 * PURPOSE:
 *   Implement concurrency and waiting admission accounting for isolated
 *   workloads.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/resilience/bulkhead.h"

#include <stddef.h>
#include <stdlib.h>
#include "umicom/platform/threading.h"
struct UmiBulkhead { UmiBulkheadConfig config; UmiBulkheadSnapshot state; UmiMutex *mutex; };
UmiStatus umi_bulkhead_create(const UmiBulkheadConfig *config, UmiBulkhead **out_bulkhead)
{
    UmiBulkhead *b; UmiStatus status;
    if (config == NULL || out_bulkhead == NULL || config->maximum_concurrent == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    *out_bulkhead = NULL; b = (UmiBulkhead *)calloc(1U, sizeof(*b)); if (b == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&b->mutex); if (status != UMI_STATUS_OK) { free(b); return status; }
    b->config = *config; *out_bulkhead = b; return UMI_STATUS_OK;
}
void umi_bulkhead_destroy(UmiBulkhead *bulkhead) { if (bulkhead != NULL) { umi_mutex_destroy(bulkhead->mutex); free(bulkhead); } }
UmiStatus umi_bulkhead_enter(UmiBulkhead *bulkhead, int allow_waiting)
{
    UmiStatus result = UMI_STATUS_OK;
    if (bulkhead == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_mutex_lock(bulkhead->mutex) != UMI_STATUS_OK) return UMI_STATUS_INTERNAL_ERROR;
    if (bulkhead->state.active < bulkhead->config.maximum_concurrent) ++bulkhead->state.active;
    else if (allow_waiting && bulkhead->state.waiting < bulkhead->config.maximum_waiting) { ++bulkhead->state.waiting; result = UMI_STATUS_BUSY; }
    else { ++bulkhead->state.rejected; result = UMI_STATUS_CAPACITY_EXCEEDED; }
    (void)umi_mutex_unlock(bulkhead->mutex); return result;
}
void umi_bulkhead_leave(UmiBulkhead *bulkhead)
{ if (bulkhead != NULL && umi_mutex_lock(bulkhead->mutex) == UMI_STATUS_OK) { if (bulkhead->state.active > 0U) --bulkhead->state.active; (void)umi_mutex_unlock(bulkhead->mutex); } }
void umi_bulkhead_waiting_done(UmiBulkhead *bulkhead)
{ if (bulkhead != NULL && umi_mutex_lock(bulkhead->mutex) == UMI_STATUS_OK) { if (bulkhead->state.waiting > 0U) --bulkhead->state.waiting; (void)umi_mutex_unlock(bulkhead->mutex); } }
UmiBulkheadSnapshot umi_bulkhead_snapshot(UmiBulkhead *bulkhead)
{ UmiBulkheadSnapshot s = {0}; if (bulkhead != NULL && umi_mutex_lock(bulkhead->mutex) == UMI_STATUS_OK) { s = bulkhead->state; (void)umi_mutex_unlock(bulkhead->mutex); } return s; }
