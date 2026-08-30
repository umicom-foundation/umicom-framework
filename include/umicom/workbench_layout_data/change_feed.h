/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/change_feed.h
 *
 * PURPOSE:
 *   Persist the ordered layout change feed used by replication, audit and downstream events.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_CHANGE_FEED_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_CHANGE_FEED_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/workbench_layout_data/record_repository.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchLayoutChangeFeedRepository {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataRecordRepository records;
} UmiWorkbenchLayoutChangeFeedRepository;

UmiStatus umi_workbench_layout_change_feed_encode(
    const UmiWorkbenchLayoutChange *record,
    char *buffer,
    size_t capacity,
    size_t *out_required);

UmiStatus umi_workbench_layout_change_feed_decode(
    const char *value,
    UmiWorkbenchLayoutChange *out_record);

UmiStatus umi_workbench_layout_change_feed_repository_init(
    UmiWorkbenchLayoutChangeFeedRepository *repository,
    UmiDataServer *server);

UmiStatus umi_workbench_layout_change_feed_save(
    const UmiWorkbenchLayoutChangeFeedRepository *repository,
    const UmiWorkbenchLayoutChange *record);

UmiStatus umi_workbench_layout_change_feed_load(
    const UmiWorkbenchLayoutChangeFeedRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutChange *out_record);

UmiStatus umi_workbench_layout_change_feed_delete(
    const UmiWorkbenchLayoutChangeFeedRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence);

UmiStatus umi_workbench_layout_change_feed_list(
    const UmiWorkbenchLayoutChangeFeedRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutChange *records,
    size_t capacity,
    size_t *out_count,
    size_t *out_total);

#ifdef __cplusplus
}
#endif

#endif
