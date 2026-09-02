/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/conflict_store.h
 *
 * PURPOSE:
 *   Persist synchronisation conflicts and explicit human or policy-driven resolution evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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


/**
 * Represent the workbench layout conflict store repository data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchLayoutConflictStoreRepository {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataRecordRepository records;
} UmiWorkbenchLayoutConflictStoreRepository;

/**
 * Write workbench layout conflict store in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_workbench_layout_conflict_store_encode(
    const UmiWorkbenchLayoutConflict *record,
    char *buffer,
    size_t capacity,
    size_t *out_required);

/**
 * Read workbench layout conflict store into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_workbench_layout_conflict_store_decode(
    const char *value,
    UmiWorkbenchLayoutConflict *out_record);

/**
 * Initialise workbench layout conflict store repository from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_workbench_layout_conflict_store_repository_init(
    UmiWorkbenchLayoutConflictStoreRepository *repository,
    UmiDataServer *server);

/**
 * Write workbench layout conflict store in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_workbench_layout_conflict_store_save(
    const UmiWorkbenchLayoutConflictStoreRepository *repository,
    const UmiWorkbenchLayoutConflict *record);

/**
 * Read workbench layout conflict store into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_workbench_layout_conflict_store_load(
    const UmiWorkbenchLayoutConflictStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutConflict *out_record);

/**
 * Provide the workbench layout conflict store delete operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_conflict_store_delete(
    const UmiWorkbenchLayoutConflictStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence);

/**
 * Provide the workbench layout conflict store list operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_conflict_store_list(
    const UmiWorkbenchLayoutConflictStoreRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutConflict *records,
    size_t capacity,
    size_t *out_count,
    size_t *out_total);

/**
 * Provide the workbench layout conflict store resolve operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_layout_conflict_store_resolve(
    const UmiWorkbenchLayoutConflictStoreRepository *repository,
    UmiWorkbenchLayoutConflict *conflict,
    UmiWorkbenchLayoutDataConflictState resolution,
    const char *resolved_by,
    uint64_t resolved_at_ms);
/**
 * Return the number of records represented by workbench layout conflict store open without
 * changing their state.
 */
size_t umi_workbench_layout_conflict_store_open_count(
    const UmiWorkbenchLayoutConflictStoreRepository *repository);

#ifdef __cplusplus
}
#endif

#endif
