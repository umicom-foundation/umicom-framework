/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/history.h
 *
 * PURPOSE:
 *   Provide a bounded thread-safe history of completed build operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_HISTORY_H
#define UMICOM_BUILD_HISTORY_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/build/result.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiBuildHistory UmiBuildHistory;

UmiStatus umi_build_history_create(size_t capacity,
                                   UmiBuildHistory **out_history);
void umi_build_history_destroy(UmiBuildHistory *history);
UmiStatus umi_build_history_append(UmiBuildHistory *history,
                                   const UmiBuildResult *result);
size_t umi_build_history_count(const UmiBuildHistory *history);
UmiStatus umi_build_history_at(const UmiBuildHistory *history,
                               size_t index,
                               UmiBuildResult *out_result);
UmiStatus umi_build_history_latest(const UmiBuildHistory *history,
                                   UmiBuildResult *out_result);
void umi_build_history_clear(UmiBuildHistory *history);
/** Reserve an identity shared by every producer of this history. Clearing the
 * retained records does not reuse old operation IDs. Output is unchanged when
 * the 64-bit identity space is exhausted. */
UmiStatus UmiBuildHistoryReserveOperationId(UmiBuildHistory *history, uint64_t *outId);

/** Copy the newest retained records in chronological order under one history
 * lock. This avoids combining count/at calls from different ring generations.
 * capacity is 1..UMI_BUILD_HISTORY_MAX; outResults must hold that many records.
 * outCount/outOmitted are required, distinct pointers and are zeroed first.
 * outOmitted counts older records still retained in this history, not records
 * already evicted by its capacity. No producer pointer is retained. The owner
 * must keep history alive until this call returns. Outputs must not alias it. */
UmiStatus UmiBuildHistoryCopyRecent(const UmiBuildHistory *history,
    UmiBuildResult *outResults, size_t capacity,
    size_t *outCount, size_t *outOmitted);

#ifdef __cplusplus
}
#endif

#endif
