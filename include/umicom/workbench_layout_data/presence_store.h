/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/presence_store.h
 *
 * PURPOSE:
 *   Persist short-lived collaboration presence and active-editor heartbeats.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_PRESENCE_STORE_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_PRESENCE_STORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/workbench_layout_data/record_repository.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench layout presence store repository data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchLayoutPresenceStoreRepository {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataRecordRepository records;
} UmiWorkbenchLayoutPresenceStoreRepository;

/**
 * Write workbench layout presence store in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_workbench_layout_presence_store_encode(
    const UmiWorkbenchLayoutPresence *record,
    char *buffer,
    size_t capacity,
    size_t *out_required);

/**
 * Read workbench layout presence store into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_workbench_layout_presence_store_decode(
    const char *value,
    UmiWorkbenchLayoutPresence *out_record);

/**
 * Initialise workbench layout presence store repository from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_workbench_layout_presence_store_repository_init(
    UmiWorkbenchLayoutPresenceStoreRepository *repository,
    UmiDataServer *server);

/**
 * Write workbench layout presence store in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_workbench_layout_presence_store_save(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
    const UmiWorkbenchLayoutPresence *record);

/**
 * Read workbench layout presence store into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_workbench_layout_presence_store_load(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutPresence *out_record);

/**
 * Provide the workbench layout presence store delete operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_presence_store_delete(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence);

/**
 * Provide the workbench layout presence store list operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_presence_store_list(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutPresence *records,
    size_t capacity,
    size_t *out_count,
    size_t *out_total);

/**
 * Provide the workbench layout presence store heartbeat operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_presence_store_heartbeat(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
    UmiWorkbenchLayoutPresence *presence,
    const char *active_node_id,
    bool editing,
    bool synchronising,
    uint64_t now_ms,
    uint64_t ttl_ms);
/**
 * Provide the workbench layout presence store expire operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_presence_store_expire(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
    uint64_t now_ms,
    size_t *out_expired);
/**
 * Return the number of records represented by workbench layout presence store active
 * without changing their state.
 */
size_t umi_workbench_layout_presence_store_active_count(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
    uint64_t now_ms);

#ifdef __cplusplus
}
#endif

#endif
