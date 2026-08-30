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


typedef struct UmiWorkbenchLayoutPresenceStoreRepository {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataRecordRepository records;
} UmiWorkbenchLayoutPresenceStoreRepository;

UmiStatus umi_workbench_layout_presence_store_encode(
    const UmiWorkbenchLayoutPresence *record,
    char *buffer,
    size_t capacity,
    size_t *out_required);

UmiStatus umi_workbench_layout_presence_store_decode(
    const char *value,
    UmiWorkbenchLayoutPresence *out_record);

UmiStatus umi_workbench_layout_presence_store_repository_init(
    UmiWorkbenchLayoutPresenceStoreRepository *repository,
    UmiDataServer *server);

UmiStatus umi_workbench_layout_presence_store_save(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
    const UmiWorkbenchLayoutPresence *record);

UmiStatus umi_workbench_layout_presence_store_load(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutPresence *out_record);

UmiStatus umi_workbench_layout_presence_store_delete(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence);

UmiStatus umi_workbench_layout_presence_store_list(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutPresence *records,
    size_t capacity,
    size_t *out_count,
    size_t *out_total);

UmiStatus umi_workbench_layout_presence_store_heartbeat(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
    UmiWorkbenchLayoutPresence *presence,
    const char *active_node_id,
    bool editing,
    bool synchronising,
    uint64_t now_ms,
    uint64_t ttl_ms);
UmiStatus umi_workbench_layout_presence_store_expire(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
    uint64_t now_ms,
    size_t *out_expired);
size_t umi_workbench_layout_presence_store_active_count(
    const UmiWorkbenchLayoutPresenceStoreRepository *repository,
    uint64_t now_ms);

#ifdef __cplusplus
}
#endif

#endif
