/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/revision_store.h
 *
 * PURPOSE:
 *   Persist immutable layout revision evidence and expose bounded history queries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_REVISION_STORE_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_REVISION_STORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/workbench_layout_data/record_repository.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench layout revision store repository data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchLayoutRevisionStoreRepository {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataRecordRepository records;
} UmiWorkbenchLayoutRevisionStoreRepository;

/**
 * Write workbench layout revision store in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_workbench_layout_revision_store_encode(
    const UmiWorkbenchLayoutRevisionRecord *record,
    char *buffer,
    size_t capacity,
    size_t *out_required);

/**
 * Read workbench layout revision store into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_workbench_layout_revision_store_decode(
    const char *value,
    UmiWorkbenchLayoutRevisionRecord *out_record);

/**
 * Initialise workbench layout revision store repository from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_workbench_layout_revision_store_repository_init(
    UmiWorkbenchLayoutRevisionStoreRepository *repository,
    UmiDataServer *server);

/**
 * Write workbench layout revision store in its stable representation and report capacity
 * or input failures to the caller.
 */
UmiStatus umi_workbench_layout_revision_store_save(
    const UmiWorkbenchLayoutRevisionStoreRepository *repository,
    const UmiWorkbenchLayoutRevisionRecord *record);

/**
 * Read workbench layout revision store into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_workbench_layout_revision_store_load(
    const UmiWorkbenchLayoutRevisionStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutRevisionRecord *out_record);

/**
 * Provide the workbench layout revision store delete operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_revision_store_delete(
    const UmiWorkbenchLayoutRevisionStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence);

/**
 * Provide the workbench layout revision store list operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_revision_store_list(
    const UmiWorkbenchLayoutRevisionStoreRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutRevisionRecord *records,
    size_t capacity,
    size_t *out_count,
    size_t *out_total);

#ifdef __cplusplus
}
#endif

#endif
