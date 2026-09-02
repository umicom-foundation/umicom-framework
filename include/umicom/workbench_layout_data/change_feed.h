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


/**
 * Represent the workbench layout change feed repository data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchLayoutChangeFeedRepository {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataRecordRepository records;
} UmiWorkbenchLayoutChangeFeedRepository;

/**
 * Write workbench layout change feed in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_workbench_layout_change_feed_encode(
    const UmiWorkbenchLayoutChange *record,
    char *buffer,
    size_t capacity,
    size_t *out_required);

/**
 * Read workbench layout change feed into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_workbench_layout_change_feed_decode(
    const char *value,
    UmiWorkbenchLayoutChange *out_record);

/**
 * Initialise workbench layout change feed repository from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_workbench_layout_change_feed_repository_init(
    UmiWorkbenchLayoutChangeFeedRepository *repository,
    UmiDataServer *server);

/**
 * Write workbench layout change feed in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_workbench_layout_change_feed_save(
    const UmiWorkbenchLayoutChangeFeedRepository *repository,
    const UmiWorkbenchLayoutChange *record);

/**
 * Read workbench layout change feed into validated module state and return a status when
 * input cannot be used.
 */
UmiStatus umi_workbench_layout_change_feed_load(
    const UmiWorkbenchLayoutChangeFeedRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutChange *out_record);

/**
 * Provide the workbench layout change feed delete operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_change_feed_delete(
    const UmiWorkbenchLayoutChangeFeedRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence);

/**
 * Provide the workbench layout change feed list operation used by this module and its
 * client applications.
 */
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
