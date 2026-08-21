/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/lease_store.h
 *
 * PURPOSE:
 *   Persist edit leases with expiry, renewal and monotonically increasing fencing tokens.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_LEASE_STORE_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_LEASE_STORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/workbench_layout_data/record_repository.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchLayoutLeaseStoreRepository {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataRecordRepository records;
} UmiWorkbenchLayoutLeaseStoreRepository;

UmiStatus umi_workbench_layout_lease_store_encode(
    const UmiWorkbenchLayoutLease *record,
    char *buffer,
    size_t capacity,
    size_t *out_required);

UmiStatus umi_workbench_layout_lease_store_decode(
    const char *value,
    UmiWorkbenchLayoutLease *out_record);

UmiStatus umi_workbench_layout_lease_store_repository_init(
    UmiWorkbenchLayoutLeaseStoreRepository *repository,
    UmiDataServer *server);

UmiStatus umi_workbench_layout_lease_store_save(
    const UmiWorkbenchLayoutLeaseStoreRepository *repository,
    const UmiWorkbenchLayoutLease *record);

UmiStatus umi_workbench_layout_lease_store_load(
    const UmiWorkbenchLayoutLeaseStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutLease *out_record);

UmiStatus umi_workbench_layout_lease_store_delete(
    const UmiWorkbenchLayoutLeaseStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence);

UmiStatus umi_workbench_layout_lease_store_list(
    const UmiWorkbenchLayoutLeaseStoreRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutLease *records,
    size_t capacity,
    size_t *out_count,
    size_t *out_total);

UmiStatus umi_workbench_layout_lease_store_acquire(
    const UmiWorkbenchLayoutLeaseStoreRepository *repository,
    const char *layout_id,
    const char *holder_id,
    const char *client_id,
    uint64_t now_ms,
    uint64_t duration_ms,
    UmiWorkbenchLayoutLease *out_lease);
UmiStatus umi_workbench_layout_lease_store_renew(
    const UmiWorkbenchLayoutLeaseStoreRepository *repository,
    UmiWorkbenchLayoutLease *lease,
    uint64_t now_ms,
    uint64_t duration_ms);
UmiStatus umi_workbench_layout_lease_store_release(
    const UmiWorkbenchLayoutLeaseStoreRepository *repository,
    UmiWorkbenchLayoutLease *lease,
    uint64_t now_ms);
bool umi_workbench_layout_lease_store_allows_write(
    const UmiWorkbenchLayoutLease *lease,
    const char *holder_id,
    const char *client_id,
    uint64_t fencing_token,
    uint64_t now_ms);

#ifdef __cplusplus
}
#endif

#endif
