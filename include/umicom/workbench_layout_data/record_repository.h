/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/record_repository.h
 *
 * PURPOSE:
 *   Provide a bounded generic repository for typed layout persistence records stored through the authoritative Data Server.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_RECORD_REPOSITORY_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_RECORD_REPOSITORY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/workbench_layout_data/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef UmiStatus (*UmiWorkbenchLayoutDataRecordDecoder)(
    const char *value,
    void *out_record);

typedef UmiStatus (*UmiWorkbenchLayoutDataRecordEncoder)(
    const void *record,
    char *buffer,
    size_t capacity,
    size_t *out_required);

typedef UmiStatus (*UmiWorkbenchLayoutDataRecordPredicate)(
    const void *record,
    void *context,
    bool *out_matches);

typedef struct UmiWorkbenchLayoutDataRecordRepository {
    uint32_t structure_size;
    UmiDataServer *server;
    UmiWorkbenchLayoutDataRecordKind kind;
    size_t record_size;
    UmiWorkbenchLayoutDataRecordEncoder encode;
    UmiWorkbenchLayoutDataRecordDecoder decode;
} UmiWorkbenchLayoutDataRecordRepository;

typedef struct UmiWorkbenchLayoutDataRecordPage {
    uint32_t structure_size;
    void *records;
    size_t record_size;
    size_t capacity;
    size_t count;
    size_t total_available;
    bool truncated;
} UmiWorkbenchLayoutDataRecordPage;

UmiStatus umi_workbench_layout_data_record_repository_init(
    UmiWorkbenchLayoutDataRecordRepository *repository,
    UmiDataServer *server,
    UmiWorkbenchLayoutDataRecordKind kind,
    size_t record_size,
    UmiWorkbenchLayoutDataRecordEncoder encode,
    UmiWorkbenchLayoutDataRecordDecoder decode);

UmiStatus umi_workbench_layout_data_record_repository_save(
    const UmiWorkbenchLayoutDataRecordRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    const void *record);

UmiStatus umi_workbench_layout_data_record_repository_load(
    const UmiWorkbenchLayoutDataRecordRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    void *out_record);

UmiStatus umi_workbench_layout_data_record_repository_delete(
    const UmiWorkbenchLayoutDataRecordRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence);

UmiStatus umi_workbench_layout_data_record_repository_list(
    const UmiWorkbenchLayoutDataRecordRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutDataRecordPage *page);

#ifdef __cplusplus
}
#endif

#endif
