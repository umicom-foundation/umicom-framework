/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/migration_store.h
 *
 * PURPOSE:
 *   Persist schema migration state and evidence for safe restart and rollback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_MIGRATION_STORE_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_MIGRATION_STORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/workbench_layout_data/record_repository.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchLayoutMigrationStoreRepository {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataRecordRepository records;
} UmiWorkbenchLayoutMigrationStoreRepository;

UmiStatus umi_workbench_layout_migration_store_encode(
    const UmiWorkbenchLayoutMigrationRecord *record,
    char *buffer,
    size_t capacity,
    size_t *out_required);

UmiStatus umi_workbench_layout_migration_store_decode(
    const char *value,
    UmiWorkbenchLayoutMigrationRecord *out_record);

UmiStatus umi_workbench_layout_migration_store_repository_init(
    UmiWorkbenchLayoutMigrationStoreRepository *repository,
    UmiDataServer *server);

UmiStatus umi_workbench_layout_migration_store_save(
    const UmiWorkbenchLayoutMigrationStoreRepository *repository,
    const UmiWorkbenchLayoutMigrationRecord *record);

UmiStatus umi_workbench_layout_migration_store_load(
    const UmiWorkbenchLayoutMigrationStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutMigrationRecord *out_record);

UmiStatus umi_workbench_layout_migration_store_delete(
    const UmiWorkbenchLayoutMigrationStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence);

UmiStatus umi_workbench_layout_migration_store_list(
    const UmiWorkbenchLayoutMigrationStoreRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutMigrationRecord *records,
    size_t capacity,
    size_t *out_count,
    size_t *out_total);

#ifdef __cplusplus
}
#endif

#endif
