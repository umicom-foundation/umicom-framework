/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/offline_queue.h
 *
 * PURPOSE:
 *   Persist offline layout operations for deterministic replay after connectivity returns.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_OFFLINE_QUEUE_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_OFFLINE_QUEUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/workbench_layout_data/record_repository.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench layout offline queue repository data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchLayoutOfflineQueueRepository {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataRecordRepository records;
} UmiWorkbenchLayoutOfflineQueueRepository;

/**
 * Write workbench layout offline queue in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_workbench_layout_offline_queue_encode(
    const UmiWorkbenchLayoutOfflineOperation *record,
    char *buffer,
    size_t capacity,
    size_t *out_required);

/**
 * Read workbench layout offline queue into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_workbench_layout_offline_queue_decode(
    const char *value,
    UmiWorkbenchLayoutOfflineOperation *out_record);

/**
 * Initialise workbench layout offline queue repository from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_workbench_layout_offline_queue_repository_init(
    UmiWorkbenchLayoutOfflineQueueRepository *repository,
    UmiDataServer *server);

/**
 * Write workbench layout offline queue in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_workbench_layout_offline_queue_save(
    const UmiWorkbenchLayoutOfflineQueueRepository *repository,
    const UmiWorkbenchLayoutOfflineOperation *record);

/**
 * Read workbench layout offline queue into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_workbench_layout_offline_queue_load(
    const UmiWorkbenchLayoutOfflineQueueRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutOfflineOperation *out_record);

/**
 * Provide the workbench layout offline queue delete operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_offline_queue_delete(
    const UmiWorkbenchLayoutOfflineQueueRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence);

/**
 * Provide the workbench layout offline queue list operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_offline_queue_list(
    const UmiWorkbenchLayoutOfflineQueueRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutOfflineOperation *records,
    size_t capacity,
    size_t *out_count,
    size_t *out_total);

/**
 * Provide the workbench layout offline queue claim available operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_layout_offline_queue_claim_available(
    const UmiWorkbenchLayoutOfflineQueueRepository *repository,
    uint64_t now_ms,
    uint64_t lease_duration_ms,
    UmiWorkbenchLayoutOfflineOperation *out_operation);
/**
 * Provide the workbench layout offline queue complete operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_offline_queue_complete(
    const UmiWorkbenchLayoutOfflineQueueRepository *repository,
    UmiWorkbenchLayoutOfflineOperation *operation);
/**
 * Provide the workbench layout offline queue retry operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_offline_queue_retry(
    const UmiWorkbenchLayoutOfflineQueueRepository *repository,
    UmiWorkbenchLayoutOfflineOperation *operation,
    uint64_t now_ms,
    uint64_t retry_delay_ms,
    const char *error);

#ifdef __cplusplus
}
#endif

#endif
