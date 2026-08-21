/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout_data/outbox.h
 *
 * PURPOSE:
 *   Persist transactional outbox work with delivery leases, retries and dead-letter evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_DATA_OUTBOX_H
#define UMICOM_WORKBENCH_LAYOUT_DATA_OUTBOX_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/workbench_layout_data/record_repository.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchLayoutOutboxRepository {
    uint32_t structure_size;
    UmiWorkbenchLayoutDataRecordRepository records;
} UmiWorkbenchLayoutOutboxRepository;

UmiStatus umi_workbench_layout_outbox_encode(
    const UmiWorkbenchLayoutOutboxEntry *record,
    char *buffer,
    size_t capacity,
    size_t *out_required);

UmiStatus umi_workbench_layout_outbox_decode(
    const char *value,
    UmiWorkbenchLayoutOutboxEntry *out_record);

UmiStatus umi_workbench_layout_outbox_repository_init(
    UmiWorkbenchLayoutOutboxRepository *repository,
    UmiDataServer *server);

UmiStatus umi_workbench_layout_outbox_save(
    const UmiWorkbenchLayoutOutboxRepository *repository,
    const UmiWorkbenchLayoutOutboxEntry *record);

UmiStatus umi_workbench_layout_outbox_load(
    const UmiWorkbenchLayoutOutboxRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence,
    UmiWorkbenchLayoutOutboxEntry *out_record);

UmiStatus umi_workbench_layout_outbox_delete(
    const UmiWorkbenchLayoutOutboxRepository *repository,
    const char *aggregate_id,
    const char *record_id,
    uint64_t sequence);

UmiStatus umi_workbench_layout_outbox_list(
    const UmiWorkbenchLayoutOutboxRepository *repository,
    const char *aggregate_id,
    UmiWorkbenchLayoutDataRecordPredicate predicate,
    void *predicate_context,
    UmiWorkbenchLayoutOutboxEntry *records,
    size_t capacity,
    size_t *out_count,
    size_t *out_total);

UmiStatus umi_workbench_layout_outbox_claim_available(
    const UmiWorkbenchLayoutOutboxRepository *repository,
    uint64_t now_ms,
    uint64_t lease_duration_ms,
    UmiWorkbenchLayoutOutboxEntry *out_entry);
UmiStatus umi_workbench_layout_outbox_mark_delivered(
    const UmiWorkbenchLayoutOutboxRepository *repository,
    UmiWorkbenchLayoutOutboxEntry *entry,
    uint64_t delivered_at_ms);
UmiStatus umi_workbench_layout_outbox_mark_failed(
    const UmiWorkbenchLayoutOutboxRepository *repository,
    UmiWorkbenchLayoutOutboxEntry *entry,
    uint64_t now_ms,
    uint64_t retry_delay_ms,
    const char *error);

#ifdef __cplusplus
}
#endif

#endif
