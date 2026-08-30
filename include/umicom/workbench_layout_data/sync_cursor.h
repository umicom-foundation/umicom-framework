/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/sync_cursor.h
 *
 * PURPOSE:
 *   Persist per-peer replication cursors and last-success evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_SYNC_CURSOR_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_SYNC_CURSOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/workbench_layout_data/record_repository.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchLayoutSyncCursorRepository {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataRecordRepository records;
} UmiWorkbenchLayoutSyncCursorRepository;

UmiStatus umi_workbench_layout_sync_cursor_encode(
    const UmiWorkbenchLayoutSyncCursor *record,
    char *buffer,
    size_t capacity,
    size_t *out_required);

UmiStatus umi_workbench_layout_sync_cursor_decode(
    const char *value,
    UmiWorkbenchLayoutSyncCursor *out_record);

UmiStatus umi_workbench_layout_sync_cursor_repository_init(
    UmiWorkbenchLayoutSyncCursorRepository *repository,
    UmiDataServer *server);

UmiStatus umi_workbench_layout_sync_cursor_save(
    const UmiWorkbenchLayoutSyncCursorRepository *repository,
    const UmiWorkbenchLayoutSyncCursor *record);

UmiStatus umi_workbench_layout_sync_cursor_load(
    const UmiWorkbenchLayoutSyncCursorRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutSyncCursor *out_record);

UmiStatus umi_workbench_layout_sync_cursor_delete(
    const UmiWorkbenchLayoutSyncCursorRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence);

UmiStatus umi_workbench_layout_sync_cursor_list(
    const UmiWorkbenchLayoutSyncCursorRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutSyncCursor *records,
    size_t capacity,
    size_t *out_count,
    size_t *out_total);

UmiStatus umi_workbench_layout_sync_cursor_record_attempt(
    const UmiWorkbenchLayoutSyncCursorRepository *repository,
    UmiWorkbenchLayoutSyncCursor *cursor,
    uint64_t attempted_at_ms,
    const char *error);
UmiStatus umi_workbench_layout_sync_cursor_record_success(
    const UmiWorkbenchLayoutSyncCursorRepository *repository,
    UmiWorkbenchLayoutSyncCursor *cursor,
    uint64_t pulled_sequence,
    uint64_t pushed_sequence,
    uint64_t succeeded_at_ms);

#ifdef __cplusplus
}
#endif

#endif
