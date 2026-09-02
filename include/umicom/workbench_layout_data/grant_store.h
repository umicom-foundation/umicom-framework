/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/grant_store.h
 *
 * PURPOSE:
 *   Persist layout sharing grants and evaluate time-bounded permissions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_GRANT_STORE_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_GRANT_STORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/workbench_layout_data/record_repository.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench layout grant store repository data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchLayoutGrantStoreRepository {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataRecordRepository records;
} UmiWorkbenchLayoutGrantStoreRepository;

/**
 * Write workbench layout grant store in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_workbench_layout_grant_store_encode(
    const UmiWorkbenchLayoutGrant *record,
    char *buffer,
    size_t capacity,
    size_t *out_required);

/**
 * Read workbench layout grant store into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_workbench_layout_grant_store_decode(
    const char *value,
    UmiWorkbenchLayoutGrant *out_record);

/**
 * Initialise workbench layout grant store repository from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_workbench_layout_grant_store_repository_init(
    UmiWorkbenchLayoutGrantStoreRepository *repository,
    UmiDataServer *server);

/**
 * Write workbench layout grant store in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_workbench_layout_grant_store_save(
    const UmiWorkbenchLayoutGrantStoreRepository *repository,
    const UmiWorkbenchLayoutGrant *record);

/**
 * Read workbench layout grant store into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_workbench_layout_grant_store_load(
    const UmiWorkbenchLayoutGrantStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutGrant *out_record);

/**
 * Provide the workbench layout grant store delete operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_grant_store_delete(
    const UmiWorkbenchLayoutGrantStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence);

/**
 * Provide the workbench layout grant store list operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_grant_store_list(
    const UmiWorkbenchLayoutGrantStoreRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutGrant *records,
    size_t capacity,
    size_t *out_count,
    size_t *out_total);

/**
 * Provide the workbench layout grant store evaluate operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_grant_store_evaluate(
    const UmiWorkbenchLayoutGrantStoreRepository *repository,
    const char *layout_id,
    const char *subject_id,
    UmiWorkbenchLayoutDataPermission permission,
    uint64_t now_ms,
    bool *out_allowed);
/**
 * Provide the workbench layout grant store revoke operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_grant_store_revoke(
    const UmiWorkbenchLayoutGrantStoreRepository *repository,
    const char *layout_id,
    const char *grant_id);

#ifdef __cplusplus
}
#endif

#endif
