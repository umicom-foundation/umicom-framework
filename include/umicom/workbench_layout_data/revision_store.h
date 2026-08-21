/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/revision_store.h
 *
 * PURPOSE:
 *   Persist immutable layout revision evidence and expose bounded history queries.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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


typedef struct UmiWorkbenchLayoutRevisionStoreRepository {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataRecordRepository records;
} UmiWorkbenchLayoutRevisionStoreRepository;

UmiStatus umi_workbench_layout_revision_store_encode(
    const UmiWorkbenchLayoutRevisionRecord *record,
    char *buffer,
    size_t capacity,
    size_t *out_required);

UmiStatus umi_workbench_layout_revision_store_decode(
    const char *value,
    UmiWorkbenchLayoutRevisionRecord *out_record);

UmiStatus umi_workbench_layout_revision_store_repository_init(
    UmiWorkbenchLayoutRevisionStoreRepository *repository,
    UmiDataServer *server);

UmiStatus umi_workbench_layout_revision_store_save(
    const UmiWorkbenchLayoutRevisionStoreRepository *repository,
    const UmiWorkbenchLayoutRevisionRecord *record);

UmiStatus umi_workbench_layout_revision_store_load(
    const UmiWorkbenchLayoutRevisionStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutRevisionRecord *out_record);

UmiStatus umi_workbench_layout_revision_store_delete(
    const UmiWorkbenchLayoutRevisionStoreRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence);

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
