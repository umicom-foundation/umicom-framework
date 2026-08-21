/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/conflict_store.h
 *
 * PURPOSE:
 *   Persist synchronisation conflicts and explicit human or policy-driven resolution evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_CONFLICT_STORE_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_CONFLICT_STORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/workbench_layout_data/record_repository.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchLayoutConflictStoreRepository {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataRecordRepository records;
} UmiWorkbenchLayoutConflictStoreRepository;

UmiStatus umi_workbench_layout_conflict_store_encode(
    const UmiWorkbenchLayoutConflict *record,
    char *buffer,
    size_t capacity,
    size_t *out_required);

UmiStatus umi_workbench_layout_conflict_store_decode(
    const char *value,
    UmiWorkbenchLayoutConflict *out_record);

UmiStatus umi_workbench_layout_conflict_store_repository_init(
    UmiWorkbenchLayoutConflictStoreRepository *repository,
    UmiDataServer *server);

UmiStatus umi_workbench_layout_conflict_store_save(
    const UmiWorkbenchLayoutConflictStoreRepository *repository,
    const UmiWorkbenchLayoutConflict *record);

UmiStatus umi_workbench_layout_conflict_store_load(
    const UmiWorkbenchLayoutConflictStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutConflict *out_record);

UmiStatus umi_workbench_layout_conflict_store_delete(
    const UmiWorkbenchLayoutConflictStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence);

UmiStatus umi_workbench_layout_conflict_store_list(
    const UmiWorkbenchLayoutConflictStoreRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutConflict *records,
    size_t capacity,
    size_t *out_count,
    size_t *out_total);

UmiStatus umi_workbench_layout_conflict_store_resolve(
    const UmiWorkbenchLayoutConflictStoreRepository *repository,
    UmiWorkbenchLayoutConflict *conflict,
    UmiWorkbenchLayoutDataConflictState resolution,
    const char *resolved_by,
    uint64_t resolved_at_ms);
size_t umi_workbench_layout_conflict_store_open_count(
    const UmiWorkbenchLayoutConflictStoreRepository *repository);

#ifdef __cplusplus
}
#endif

#endif
