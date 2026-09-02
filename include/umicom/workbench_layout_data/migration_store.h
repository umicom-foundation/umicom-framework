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


/**
 * Represent the workbench layout migration store repository data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchLayoutMigrationStoreRepository {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataRecordRepository records;
} UmiWorkbenchLayoutMigrationStoreRepository;

/**
 * Write workbench layout migration store in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_workbench_layout_migration_store_encode(
    const UmiWorkbenchLayoutMigrationRecord *record,
    char *buffer,
    size_t capacity,
    size_t *out_required);

/**
 * Read workbench layout migration store into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_workbench_layout_migration_store_decode(
    const char *value,
    UmiWorkbenchLayoutMigrationRecord *out_record);

/**
 * Initialise workbench layout migration store repository from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_workbench_layout_migration_store_repository_init(
    UmiWorkbenchLayoutMigrationStoreRepository *repository,
    UmiDataServer *server);

/**
 * Write workbench layout migration store in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_workbench_layout_migration_store_save(
    const UmiWorkbenchLayoutMigrationStoreRepository *repository,
    const UmiWorkbenchLayoutMigrationRecord *record);

/**
 * Read workbench layout migration store into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_workbench_layout_migration_store_load(
    const UmiWorkbenchLayoutMigrationStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutMigrationRecord *out_record);

/**
 * Provide the workbench layout migration store delete operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_migration_store_delete(
    const UmiWorkbenchLayoutMigrationStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence);

/**
 * Provide the workbench layout migration store list operation used by this module and its
 * client applications.
 */
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
